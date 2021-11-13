#include <mos/apu/source.hpp>
#include <mos/aud/source.hpp>

namespace mos::apu {

Source::Source(const aud::Source &source)
    : position(source.position), velocity(source.velocity), pitch(source.pitch),
      gain(source.gain), loop(source.loop), playing(source.playing),
      obstructed(source.obstructed), id_(source.id()) {}

auto Source::id() const -> unsigned int { return id_; }

} // namespace mos::apu
