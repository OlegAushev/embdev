#include <emb/dev/hall/emulator.hpp>

namespace emb::dev::hall {

sector emulated_sector(emb::units::erad_f32 angle, sector current)
{
  return detail::sector_at(angle, current);
}

} // namespace emb::dev::hall
