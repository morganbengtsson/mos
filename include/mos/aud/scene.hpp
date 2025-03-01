#pragma once

#include <mos/aud/listener.hpp>
#include <mos/aud/sound_streams.hpp>
#include <mos/aud/sounds.hpp>

namespace mos::aud {

/** Audio scene with listener and audio sources. */
class Scene final {
public:
  Scene(Sounds sounds, Sound_streams sound_streams, Listener listener);
  Scene();

  /** Sound streams in the scene.*/
  Sounds sounds;

  /** Streaming sounds in the scene. */
  Sound_streams sound_streams;

  /** The listener. */
  Listener listener;
};
} // namespace mos::aud
