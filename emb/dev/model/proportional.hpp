#pragma once

namespace emb::dev::model {

// A line through the origin, given one rated point:
//   y = rated_output * x / rated_input
// Only the linear characteristic is modelled: saturation beyond the rated
// range, offset and its drift are not.
template<typename In, typename Out>
struct proportional {
  In rated_input;
  Out rated_output;

  constexpr proportional(In rated_in, Out rated_out)
      : rated_input(rated_in), rated_output(rated_out)
  {
  }

  constexpr Out forward(In in) const
  {
    return rated_output * (in / rated_input);
  }

  constexpr In inverse(Out out) const
  {
    return rated_input * (out / rated_output);
  }
};

} // namespace emb::dev::model
