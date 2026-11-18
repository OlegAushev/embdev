#pragma once

#include <emb/dev/contactors/gpio.hpp>
#include <emb/dev/contactors/types.hpp>
#include <emb/dev/contactors/unmonitored.hpp>

#include <utility>

namespace emb::dev::contactors {

template<normal_position NormalPosition, driver Driver, feedback Feedback>
class monitored {
public:
  constexpr monitored(NormalPosition normal_pos,
                      Driver drv,
                      Feedback fdb,
                      position initial)
      : drive_(normal_pos, std::move(drv), initial), feedback_(std::move(fdb))
  {
  }

  constexpr void request(position pos)
  {
    drive_.request(pos);
  }

  constexpr void request_open()
  {
    drive_.request_open();
  }

  constexpr void request_close()
  {
    drive_.request_close();
  }

  constexpr position requested() const
  {
    return drive_.requested();
  }

  constexpr position actual() const
  {
    return NormalPosition{}(feedback_());
  }

  constexpr bool at_requested() const
  {
    return actual() == requested();
  }
private:
  unmonitored<NormalPosition, Driver> drive_;
  [[no_unique_address]] Feedback feedback_;
};

template<normal_position NormalPosition,
         emb::gpio::output ControlPin,
         emb::gpio::input FeedbackPin>
using basic_monitored = monitored<NormalPosition,
                                  gpio_driver<ControlPin>,
                                  gpio_feedback<FeedbackPin>>;

} // namespace emb::dev::contactors
