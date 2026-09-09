#pragma once

#include <emb/dev/model/pt.hpp>

#include <emb/math.hpp>
#include <emb/units.hpp>

#include <concepts>

namespace emb::dev::part {

using emb::units::degree_celsius_f32;
using emb::units::ohm_f32;

inline constexpr model::pt pt100{ohm_f32{100.f}};
inline constexpr model::pt pt1000{ohm_f32{1000.f}};

// the state is supplied from outside -- not default-constructed in transform
static_assert(!std::default_initializable<model::pt>);

// ---- compile-time self-test (device transfer only) ----

// IEC 60751 reference points
static_assert(emb::approx(pt100.forward(degree_celsius_f32{0.f}),
                          ohm_f32{100.f},
                          ohm_f32{1e-3f}));
static_assert(emb::approx(pt100.forward(degree_celsius_f32{100.f}),
                          ohm_f32{138.51f},
                          ohm_f32{0.01f}));
static_assert(emb::approx(pt100.forward(degree_celsius_f32{-60.f}),
                          ohm_f32{76.33f},
                          ohm_f32{0.01f}));
static_assert(emb::approx(pt100.forward(degree_celsius_f32{200.f}),
                          ohm_f32{175.86f},
                          ohm_f32{0.01f}));
static_assert(emb::approx(pt1000.forward(degree_celsius_f32{150.f}),
                          ohm_f32{1573.25f},
                          ohm_f32{0.1f}));

// round trip
static_assert(emb::approx(
    pt100.inverse(pt100.forward(degree_celsius_f32{25.f})).value(),
    25.f,
    0.01f));
static_assert(emb::approx(
    pt100.inverse(pt100.forward(degree_celsius_f32{-60.f})).value(),
    -60.f,
    0.01f));
static_assert(emb::approx(
    pt1000.inverse(pt1000.forward(degree_celsius_f32{219.f})).value(),
    219.f,
    0.01f));
static_assert(emb::approx(
    pt100.inverse(pt100.forward(degree_celsius_f32{-200.f})).value(),
    -200.f,
    0.05f));
static_assert(emb::approx(
    pt100.inverse(pt100.forward(degree_celsius_f32{850.f})).value(),
    850.f,
    0.05f));

} // namespace emb::dev::part
