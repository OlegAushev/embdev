#pragma once

#include <emb/units.hpp>

#include <cmath>

namespace emb::dev::model {

using emb::units::degree_celsius_f32;
using emb::units::ohm_f32;

// NTC thermistor, beta-equation model:
//   R(T) = R25 * exp(beta * (1/T - 1/T25)),  T in kelvin, T25 = 298.15 K
struct ntc {
  ohm_f32 R25; // resistance at 25 degC
  float Beta;  // beta constant, K

  constexpr ntc(ohm_f32 r25, float beta_kelvin) : R25(r25), Beta(beta_kelvin) {}

  static constexpr float T25 = 298.15f;           // K
  static constexpr float kelvin_offset = 273.15f; // K

  constexpr ohm_f32 forward(degree_celsius_f32 temp) const
  {
    float const Tk = temp.value() + kelvin_offset;
    return R25 * std::exp(Beta * (1.f / Tk - 1.f / T25));
  }

  constexpr degree_celsius_f32 inverse(ohm_f32 r) const
  {
    float const Tk_inv = 1.f / T25 + std::log(r / R25) / Beta;
    return degree_celsius_f32{1.f / Tk_inv - kelvin_offset};
  }
};

} // namespace emb::dev::model
