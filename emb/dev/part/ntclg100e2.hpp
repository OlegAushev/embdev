#pragma once

#include <algorithm>
#include <array>

namespace emb::dev::part::ntclg100e2 {

using RValues = std::array<float, 49>;

inline constexpr float start_temperature = -40.f;
inline constexpr float temperature_step = 5.f;

// clang-format off
inline constexpr RValues rt_r25 = {
    33.21f, 23.99f, 17.52f, 12.93f, 9.636f,
    7.25f, 5.505f, 4.216f, 3.255f, 2.534f,
    1.987f, 1.57f, 1.249f, 1.000f, 0.8058f,
    0.6535f, 0.5330f, 0.4372f, 0.3605f, 0.2989f,
    0.2490f, 0.2084f, 0.1753f, 0.1481f, 0.1256f,
    0.1070f, 0.09154f, 0.07860f, 0.06773f, 0.05857f,
    0.05083f, 0.04426f, 0.03866f, 0.03387f, 0.02977f,
    0.02624f, 0.02319f, 0.02055f, 0.01826f, 0.01627f,
    0.01453f, 0.01301f, 0.01167f, 0.01049f, 0.009457f,
    0.008541f, 0.007729f, 0.007009f, 0.006367f};
// clang-format on

static_assert(std::is_sorted(rt_r25.rbegin(), rt_r25.rend()));

constexpr RValues calculate_resistance_values(float r25)
{
  RValues v;
  std::transform(rt_r25.begin(), rt_r25.end(), v.begin(), [r25](float arg) {
    return arg * r25;
  });
  return v;
}

} // namespace emb::dev::part::ntclg100e2
