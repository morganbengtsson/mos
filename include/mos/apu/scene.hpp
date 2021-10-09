#pragma once

#include <mos/apu/sound.hpp>

#include <mos/aud/listener.hpp>

namespace mos::apu {

using Sounds = std::vector<Sound>;

/** Audio scene with listener and audio sources. */
class Scene final {
public:
  /** Sound streams in the scene.*/
  Sounds sounds;

  /** Streaming sounds in the scene. */
  //Sound_streams sound_streams;

  /** The listener. */
  aud::Listener listener;
};
}

