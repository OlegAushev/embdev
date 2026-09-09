#pragma once

#include <emb/dev/model/proportional.hpp>

#include <emb/math.hpp>
#include <emb/units.hpp>

#include <concepts>

namespace emb::dev::part {

using emb::units::amp_f32;
using emb::units::volt_f32;

// Coding TKC-BS/BR series: open loop, +/-15 V supply, +/-4 V at +/-Ipn.
// Measuring range +/-3 Ipn up to TKC300, +/-900 A from TKC400 on. BS is the
// pin-header variant, BR the flying-lead one; the transfer is the same.
inline constexpr model::proportional tkc50{amp_f32{50.f}, volt_f32{4.f}};
inline constexpr model::proportional tkc75{amp_f32{75.f}, volt_f32{4.f}};
inline constexpr model::proportional tkc100{amp_f32{100.f}, volt_f32{4.f}};
inline constexpr model::proportional tkc200{amp_f32{200.f}, volt_f32{4.f}};
inline constexpr model::proportional tkc300{amp_f32{300.f}, volt_f32{4.f}};
inline constexpr model::proportional tkc400{amp_f32{400.f}, volt_f32{4.f}};
inline constexpr model::proportional tkc500{amp_f32{500.f}, volt_f32{4.f}};
inline constexpr model::proportional tkc600{amp_f32{600.f}, volt_f32{4.f}};

// the state is supplied from outside -- not default-constructed in transform
static_assert(!std::default_initializable<model::proportional<amp_f32,
                                                              volt_f32>>);

// ==== compile-time self-test (device transfer only) ====

// datasheet rated points
static_assert(emb::approx(tkc600.forward(amp_f32{600.f}), volt_f32{4.f}));
static_assert(emb::approx(tkc600.forward(amp_f32{-600.f}), volt_f32{-4.f}));
static_assert(emb::approx(tkc600.forward(amp_f32{0.f}), volt_f32{0.f}));
static_assert(emb::approx(tkc100.forward(amp_f32{50.f}), volt_f32{2.f}));
static_assert(emb::approx(tkc50.inverse(volt_f32{2.f}), amp_f32{25.f}));

// round trip across the measuring range
static_assert(emb::approx(tkc600.inverse(tkc600.forward(amp_f32{900.f})),
                          amp_f32{900.f}));
static_assert(emb::approx(tkc600.inverse(tkc600.forward(amp_f32{-37.5f})),
                          amp_f32{-37.5f}));
static_assert(emb::approx(tkc300.inverse(tkc300.forward(amp_f32{-900.f})),
                          amp_f32{-900.f}));

} // namespace emb::dev::part
