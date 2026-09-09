#pragma once

#include <emb/dev/model/pt.hpp>

#include <emb/units.hpp>

#include <concepts>

namespace emb::dev::part {

using emb::units::degree_celsius_f32;
using emb::units::ohm_f32;

inline constexpr model::pt pt100{ohm_f32{100.f}};
inline constexpr model::pt pt1000{ohm_f32{1000.f}};

// the state is supplied from outside -- not default-constructed in transform
static_assert(!std::default_initializable<model::pt>);

} // namespace emb::dev::part
