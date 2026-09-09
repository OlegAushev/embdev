#include <emb/dev/part/pt.hpp>

#include <emb/math.hpp>

namespace {

using namespace emb::units;
using emb::dev::part::pt100;
using emb::dev::part::pt1000;

constexpr bool test_pt()
{
  // IEC 60751 reference points
  assert(emb::approx(pt100.forward(degree_celsius_f32{0.f}),
                     ohm_f32{100.f},
                     ohm_f32{1e-3f}));
  assert(emb::approx(pt100.forward(degree_celsius_f32{100.f}),
                     ohm_f32{138.51f},
                     ohm_f32{0.01f}));
  assert(emb::approx(pt100.forward(degree_celsius_f32{-60.f}),
                     ohm_f32{76.33f},
                     ohm_f32{0.01f}));
  assert(emb::approx(pt100.forward(degree_celsius_f32{200.f}),
                     ohm_f32{175.86f},
                     ohm_f32{0.01f}));
  assert(emb::approx(pt1000.forward(degree_celsius_f32{150.f}),
                     ohm_f32{1573.25f},
                     ohm_f32{0.1f}));

  // round trip
  assert(emb::approx(
      pt100.inverse(pt100.forward(degree_celsius_f32{25.f})).value(),
      25.f,
      0.01f));
  assert(emb::approx(
      pt100.inverse(pt100.forward(degree_celsius_f32{-60.f})).value(),
      -60.f,
      0.01f));
  assert(emb::approx(
      pt1000.inverse(pt1000.forward(degree_celsius_f32{219.f})).value(),
      219.f,
      0.01f));
  assert(emb::approx(
      pt100.inverse(pt100.forward(degree_celsius_f32{-200.f})).value(),
      -200.f,
      0.05f));
  assert(emb::approx(
      pt100.inverse(pt100.forward(degree_celsius_f32{850.f})).value(),
      850.f,
      0.05f));

  return true;
}

static_assert(test_pt());

} // namespace
