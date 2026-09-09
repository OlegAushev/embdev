#include <emb/dev/ntc.hpp>

#include <emb/math.hpp>

namespace {

using namespace emb::units;

// 10k/3435 device transfer
[[maybe_unused]] constexpr bool test_ntc()
{
  [[maybe_unused]] emb::dev::ntc const t{ohm_f32{10'000.f}, 3435.f};

  assert(emb::approx(t.forward(degree_celsius_f32{25.f}),
                     ohm_f32{10'000.f},
                     ohm_f32{0.01f}));
  assert(emb::approx(t.forward(degree_celsius_f32{100.f}),
                     ohm_f32{987.2f},
                     ohm_f32{1.f}));

  // round trip across the operating range
  assert(emb::approx(t.inverse(t.forward(degree_celsius_f32{-40.f})).value(),
                     -40.f,
                     0.05f));
  assert(emb::approx(t.inverse(t.forward(degree_celsius_f32{150.f})).value(),
                     150.f,
                     0.05f));

  return true;
}

// gcc constant-folds exp/log in constant expressions; clang (i.e. clangd)
// cannot yet, so the check runs only under the build compiler.
#if !defined(__clang__)
static_assert(test_ntc());
#endif

} // namespace
