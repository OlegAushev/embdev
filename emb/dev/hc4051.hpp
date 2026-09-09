#pragma once

#include <emb/gpio.hpp>

#include <array>
#include <cstdint>
#include <utility>

namespace emb::dev::hc4051 {

// 74HC4051 8:1 analog multiplexer inputs
enum class input : std::uint8_t { in0, in1, in2, in3, in4, in5, in6, in7 };

// The multiplexer with the address lines S0..S2 driven as plain GPIO
// outputs. The enable input is expected to be strapped active on the board;
// the common terminal follows the selected input only after the device's
// switching time plus the settling time of the circuit downstream.
template<emb::gpio::output Pin>
class mux {
private:
  Pin s0_;
  Pin s1_;
  Pin s2_;
public:
  template<typename Config>
  explicit mux(std::array<Config, 3> const& pins, input initial = input::in0)
      : s0_(pins[0]), s1_(pins[1]), s2_(pins[2])
  {
    select(initial);
  }

  void select(input in)
  {
    auto const address = std::to_underlying(in);
    s0_.set_level(bit(address, 0));
    s1_.set_level(bit(address, 1));
    s2_.set_level(bit(address, 2));
  }
private:
  static emb::gpio::level bit(std::uint8_t address, unsigned pos)
  {
    return ((address >> pos) & 1u) != 0 ? emb::gpio::level::high
                                        : emb::gpio::level::low;
  }
};

} // namespace emb::dev::hc4051
