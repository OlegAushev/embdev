#pragma once

#include <emb/dev/contactors/gpio.hpp>
#include <emb/dev/contactors/types.hpp>

#include <utility>

namespace emb::dev::contactors {

template<normal_position NormalPosition, driver Driver>
class unmonitored {
public:
  constexpr unmonitored(NormalPosition, Driver drv, position initial)
      : driver_(std::move(drv))
  {
    request(initial);
  }

  unmonitored(unmonitored const&) = delete;
  unmonitored& operator=(unmonitored const&) = delete;

  constexpr void request(position pos)
  {
    requested_ = pos;
    driver_(NormalPosition{}(pos));
  }

  constexpr void request_open()
  {
    request(position::open);
  }

  constexpr void request_close()
  {
    request(position::closed);
  }

  constexpr position requested() const
  {
    return requested_;
  }
private:
  [[no_unique_address]] Driver driver_;
  position requested_;
};

template<normal_position NormalPosition, emb::gpio::output ControlPin>
using basic_unmonitored = unmonitored<NormalPosition, gpio_driver<ControlPin>>;

} // namespace emb::dev::contactors
