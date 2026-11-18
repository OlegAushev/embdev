#pragma once

#include <emb/meta/same_as_any.hpp>

#include <concepts>
#include <cstdint>

namespace emb::dev::contactors {

enum class position : std::uint8_t {
  open,
  closed
};

enum class signal : std::uint8_t {
  inactive,
  active
};

struct normally_open {
  static constexpr signal operator()(position pos)
  {
    return pos == position::closed ? signal::active : signal::inactive;
  }

  static constexpr position operator()(signal s)
  {
    return s == signal::inactive ? position::open : position::closed;
  }
};

struct normally_closed {
  static constexpr signal operator()(position pos)
  {
    return pos == position::open ? signal::active : signal::inactive;
  }

  static constexpr position operator()(signal s)
  {
    return s == signal::inactive ? position::closed : position::open;
  }
};

template<typename T>
concept normal_position = emb::same_as_any<T, normally_open, normally_closed>;

template<typename T>
concept driver = requires(T& d) {
  { d(signal::active) } -> std::same_as<void>;
};

// A feedback source reports the sensed position as an actuation signal:
// inactive for the device's normal (de-energized) position,
// active for the opposite position.
template<typename T>
concept feedback = requires(T const& f) {
  { f() } -> std::same_as<signal>;
};

} // namespace emb::dev::contactors
