#pragma once

#include <mos/aud/sound.hpp>

#include <mos/apu/buffer.hpp>
#include <mos/apu/source.hpp>

namespace mos::al {
class Renderer;
} // namespace mos::al

namespace mos::apu {

class Sound {
  friend class mos::al::Renderer;

public:
  Buffer buffer;
  Source source;

private:
  Sound(const mos::aud::Sound &sound);;
};

} // namespace mos::apu
