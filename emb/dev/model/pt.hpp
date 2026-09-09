#pragma once

#include <emb/math.hpp>
#include <emb/units.hpp>

namespace emb::dev::model {

// Platinum RTD per IEC 60751 (alpha = 3.851e-3), Callendar-Van Dusen transfer:
//   R(T) = R0 * (1 + A*T + B*T^2)                       0 <= T <= 850 degC
//   R(T) = R0 * (1 + A*T + B*T^2 + C*(T - 100)*T^3)   -200 <= T < 0 degC
struct pt {
  emb::units::ohm_f32 R0; // resistance at 0 degC

  constexpr explicit pt(emb::units::ohm_f32 r0) : R0(r0) {}

  static constexpr float A = 3.9083e-3f;
  static constexpr float B = -5.775e-7f;
  static constexpr float C = -4.183e-12f;

  constexpr emb::units::ohm_f32
  forward(emb::units::degree_celsius_f32 temp) const
  {
    float const t = temp.value();
    float rn = 1.f + (A + B * t) * t;
    if (t < 0.f) {
      rn += C * (t - 100.f) * t * t * t;
    }
    return R0 * rn;
  }

  constexpr emb::units::degree_celsius_f32 inverse(emb::units::ohm_f32 r) const
  {
    float const rn = r / R0;
    // cancellation-free root of B*t^2 + A*t + (1 - rn) = 0, exact for t >= 0
    float t = 2.f * (rn - 1.f) / (A + emb::sqrt(A * A - 4.f * B * (1.f - rn)));
    if (t < 0.f) {
      // one Newton step absorbs the C term
      float const f = 1.f + (A + B * t) * t + C * (t - 100.f) * t * t * t - rn;
      float const df = A + 2.f * B * t + C * (4.f * t - 300.f) * t * t;
      t -= f / df;
    }
    return emb::units::degree_celsius_f32{t};
  }
};

} // namespace emb::dev::model
