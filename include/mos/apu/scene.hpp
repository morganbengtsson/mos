#pragma once

#include <mos/apu/sounds.hpp>
#include <mos/apu/sound_streams.hpp>

#include <mos/aud/listener.hpp>

namespace mos::apu {

/** Audio scene with listener and audio sources. */
class Scene final {
public:
  /** Sound streams in the scene.*/
  Sounds sounds;

  /** Streaming sounds in the scene. */
  Sound_streams sound_streams;

  /** The listener. */
  aud::Listener listener;
};
} // namespace mos::apu
