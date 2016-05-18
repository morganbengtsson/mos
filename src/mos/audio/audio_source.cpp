#include <mos/audio/audio_source.hpp>

namespace mos {

AudioSource::AudioSource(const glm::vec3 position, const glm::vec3 velocity,
               const float pitch, const float gain, const bool loop,
               const bool playing, const float obstructed, const bool outside)
    : position(position), velocity(velocity), pitch(pitch), gain(gain),
      loop(loop), playing(playing), obstructed(obstructed) {
  static unsigned int current_id = 0;
  id_ = current_id++;
}

AudioSource::~AudioSource() {}

unsigned int AudioSource::id() const { return id_; }
}
