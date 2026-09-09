#include <emb/dev/part/tkc.hpp>

#include <emb/math.hpp>

namespace {

using namespace emb::units;
using emb::dev::part::tkc100;
using emb::dev::part::tkc300;
using emb::dev::part::tkc50;
using emb::dev::part::tkc600;

constexpr bool test_tkc()
{
  // datasheet rated points, exact by construction
  assert(emb::approx(tkc600.forward(amp_f32{600.f}),
                     volt_f32{4.f},
                     volt_f32{1e-4f}));
  assert(emb::approx(tkc600.forward(amp_f32{-600.f}),
                     volt_f32{-4.f},
                     volt_f32{1e-4f}));
  assert(emb::approx(tkc600.forward(amp_f32{0.f}),
                     volt_f32{0.f},
                     volt_f32{1e-4f}));
  assert(emb::approx(tkc100.forward(amp_f32{50.f}),
                     volt_f32{2.f},
                     volt_f32{1e-4f}));
  assert(emb::approx(tkc50.inverse(volt_f32{2.f}),
                     amp_f32{25.f},
                     amp_f32{1e-2f}));

  // round trip across the measuring range
  assert(emb::approx(tkc600.inverse(tkc600.forward(amp_f32{900.f})),
                     amp_f32{900.f},
                     amp_f32{1e-2f}));
  assert(emb::approx(tkc600.inverse(tkc600.forward(amp_f32{-37.5f})),
                     amp_f32{-37.5f},
                     amp_f32{1e-2f}));
  assert(emb::approx(tkc300.inverse(tkc300.forward(amp_f32{-900.f})),
                     amp_f32{-900.f},
                     amp_f32{1e-2f}));

  return true;
}

static_assert(test_tkc());

} // namespace
