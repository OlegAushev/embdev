#pragma once

#include <emb/math.hpp>
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

// ---- compile-time self-test (device transfer only, 10k/3435 example) ----

// gcc constant-folds exp/log in constant expressions; clang (i.e. clangd)
// cannot yet, so the checks run only under the build compiler.
#if !defined(__clang__)

static_assert(emb::approx(
    ntc{ohm_f32{10'000.f}, 3435.f}.forward(degree_celsius_f32{25.f}),
    ohm_f32{10'000.f},
    0.01f));
static_assert(emb::approx(
    ntc{ohm_f32{10'000.f}, 3435.f}.forward(degree_celsius_f32{100.f}),
    ohm_f32{987.2f},
    1.f));
static_assert(emb::approx(
    ntc{ohm_f32{10'000.f}, 3435.f}
        .inverse(ntc{ohm_f32{10'000.f}, 3435.f}.forward(degree_celsius_f32{
            -40.f}))
        .value(),
    -40.f,
    0.05f));
static_assert(emb::approx(
    ntc{ohm_f32{10'000.f}, 3435.f}
        .inverse(ntc{ohm_f32{10'000.f}, 3435.f}.forward(degree_celsius_f32{
            150.f}))
        .value(),
    150.f,
    0.05f));

#endif

} // namespace emb::dev::model
