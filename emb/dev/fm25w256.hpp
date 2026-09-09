#pragma once

#include <emb/expected.hpp>
#include <emb/nvm/storage.hpp>
#include <emb/spi.hpp>

#include <algorithm>
#include <array>
#include <bit>
#include <chrono>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <optional>
#include <span>
#include <utility>

namespace emb::dev::fm25w256 {

enum class opcode_command : std::uint8_t {
  write_enable = 0x06,
  write_disable = 0x04,
  read_sr = 0x05,
  write_sr = 0x01,
  read = 0x03,
  write = 0x02
};

// Why this part refused. Its own, because the reasons are its own: a write
// that does not take is the write-enable latch refusing, which is an
// FM25W256 fact and not a property of non-volatile memory in general.
//
// `bus` is not a reason so much as a referral — the transfer never reached
// the part, and what went wrong is in the bus's vocabulary rather than
// this one. The storage concept asks for an error type and deliberately
// does not say which; whoever must collapse several into one does it at
// their own boundary.
enum class reason : std::uint8_t {
  invalid_argument, // the range is empty or outside the part
  write_disabled,   // the latch did not take: protected, or not answering
  bus,              // the transfer failed; see the cause
};

struct status_register {
  std::uint8_t _r0 : 1;
  std::uint8_t wel : 1;
  std::uint8_t bp0 : 1;
  std::uint8_t bp1 : 1;
  std::uint8_t _r1 : 1;
  std::uint8_t _r2 : 1;
  std::uint8_t _r3 : 1;
  std::uint8_t wpen : 1;
};

template<emb::spi::some_device Dev>
  requires std::same_as<typename Dev::frame_type, std::uint8_t>
class fram {
public:
  using addr_type = std::uint16_t;

  // A refusal of this part's own, or one passed up from the bus with what
  // the bus said about it. The cause is absent in the first case: nothing
  // below was asked, so nothing below has an opinion.
  struct error {
    reason why;
    std::optional<typename Dev::error_type> bus;
  };

  using error_type = error;

  // 256 Kbit, addressed 0x0000..0x7FFF: 0x8000 bytes, one more than the
  // highest address.
  static constexpr std::size_t capacity = 0x8000;
  static constexpr addr_type max_addr = 0x7FFF;

  // What emb::nvm::some_block_storage asks of a medium. Ferroelectric RAM
  // writes bytes in place, so there is no granularity to respect and no
  // erase step before a write. `erased_value` is a convention rather than a
  // hardware state — erase() writes it, so an explicit wipe leaves the same
  // pattern a flash would, and a slot scan cannot tell the two media apart.
  static constexpr std::size_t write_granularity = 1;
  static constexpr bool needs_erase = false;
  static constexpr std::byte erased_value{0xFF};

  // tCSS and tCSH. A microsecond is what this driver has always waited and
  // is orders of magnitude more than the part asks for; it is kept because
  // it is known to work, not because it was looked up. Tighten it against
  // the datasheet — the master treats both as lower bounds.
  static constexpr emb::spi::cs_timing cs_timing{
      .setup = std::chrono::microseconds{1},
      .hold = std::chrono::microseconds{1}};

  explicit fram(Dev& dev) : dev_(dev) {}

  auto read(addr_type addr, std::span<std::byte> buf)
      -> std::expected<void, error>
  {
    if (buf.empty() || !in_range(addr, buf.size())) {
      return refuse(reason::invalid_argument);
    }

    auto const header = command(opcode_command::read, addr);

    emb::spi::selection const cs{dev_, cs_timing};
    TRY(emb::spi::write_bytes(dev_, header).transform_error(from_bus));
    return emb::spi::read_bytes(dev_, buf).transform_error(from_bus);
  }

  auto write(addr_type addr, std::span<std::byte const> buf)
      -> std::expected<void, error>
  {
    if (buf.empty() || !in_range(addr, buf.size())) {
      return refuse(reason::invalid_argument);
    }

    TRY(enable_writes());

    auto const header = command(opcode_command::write, addr);

    emb::spi::selection const cs{dev_, cs_timing};
    TRY(emb::spi::write_bytes(dev_, header).transform_error(from_bus));
    return emb::spi::write_bytes(dev_, buf).transform_error(from_bus);
  }

  // Ferroelectric memory has no erased state, so this is an overwrite with
  // erased_value — which is exactly what the contract asks for: bring the
  // range to the value a scan reads as "nothing was written here".
  auto erase(addr_type addr, std::size_t size) -> std::expected<void, error>
  {
    if (size == 0 || !in_range(addr, size)) {
      return refuse(reason::invalid_argument);
    }

    TRY(enable_writes());

    auto const header = command(opcode_command::write, addr);

    // One transaction from the opcode to the last byte: the part takes the
    // address once and walks it, so the filler is streamed a chunk at a
    // time rather than held whole.
    emb::spi::selection const cs{dev_, cs_timing};
    TRY(emb::spi::write_bytes(dev_, header).transform_error(from_bus));

    for (auto left = size; left > 0;) {
      auto const n = std::min(left, filler.size());
      TRY(emb::spi::write_bytes(dev_, std::span{filler}.first(n))
              .transform_error(from_bus));
      left -= n;
    }
    return {};
  }
private:
  Dev& dev_;

  static constexpr std::size_t chunk_size = 32;

  static constexpr auto filler = []() {
    std::array<std::byte, chunk_size> a{};
    a.fill(erased_value);
    return a;
  }();

  static constexpr bool in_range(addr_type addr, std::size_t size)
  {
    return (std::size_t{addr} <= capacity) && (size <= capacity - addr);
  }

  static constexpr auto command(opcode_command op, addr_type addr)
      -> std::array<std::byte, 3>
  {
    addr = addr & max_addr;
    return {std::byte(std::to_underlying(op)),
            std::byte(addr >> 8),
            std::byte(addr & 0x00FF)};
  }

  static error from_bus(typename Dev::error_type cause)
  {
    return {reason::bus, cause};
  }

  static auto refuse(reason why) -> std::unexpected<error>
  {
    return std::unexpected(error{why, std::nullopt});
  }

  // A write has to be armed, and the arming confirmed: the latch clears
  // itself after every completed write, and a part that is protected or
  // not answering will not take it. Two transactions, because the part
  // wants the enable ended before it will report the latch.
  auto enable_writes() -> std::expected<void, error>
  {
    {
      auto const cmd = std::array{
          std::byte(std::to_underlying(opcode_command::write_enable))};
      emb::spi::selection const cs{dev_, cs_timing};
      TRY(emb::spi::write_bytes(dev_, cmd).transform_error(from_bus));
    }

    if (auto const sr = TRY(read_status_register()); !sr.wel) {
      return refuse(reason::write_disabled);
    }
    return {};
  }

  auto read_status_register() -> std::expected<status_register, error>
  {
    auto const cmd =
        std::array{std::byte(std::to_underlying(opcode_command::read_sr))};
    std::array<std::byte, 1> sr{};

    emb::spi::selection const cs{dev_, cs_timing};
    TRY(emb::spi::write_bytes(dev_, cmd).transform_error(from_bus));
    TRY(emb::spi::read_bytes(dev_, sr).transform_error(from_bus));
    return std::bit_cast<status_register>(sr[0]);
  }
};

} // namespace emb::dev::fm25w256
