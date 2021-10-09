#pragma once

#include <mos/apu/sound.hpp>
#include <mos/apu/sound_stream.hpp>

#include <mos/aud/listener.hpp>

namespace mos::apu {

using Sounds = std::vector<Sound>; // TODO: Move
using Sound_streams = std::vector<Sound_stream>; // TODO: Move

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
}

