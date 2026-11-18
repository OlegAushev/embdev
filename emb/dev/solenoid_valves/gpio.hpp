#pragma once

#include <emb/dev/solenoid_valves/types.hpp>
#include <emb/gpio.hpp>

namespace emb::dev::solenoid_valves {

template<emb::gpio::output Pin>
class gpio_driver {
private:
  Pin& pin_;
public:
  explicit gpio_driver(Pin& pin) : pin_(pin) {}

  void operator()(signal s)
  {
    pin_.set(s == signal::active ? emb::gpio::state::active
                                 : emb::gpio::state::inactive);
  }
};

template<emb::gpio::input Pin>
class gpio_feedback {
private:
  Pin& pin_;
public:
  explicit gpio_feedback(Pin& pin) : pin_(pin) {}

  signal operator()() const
  {
    return pin_.read() == emb::gpio::state::active ? signal::active
                                                   : signal::inactive;
  }
};

} // namespace emb::dev::solenoid_valves
