#include <mos/audio/audio_source.hpp>

namespace mos {

std::atomic_uint AudioSource::current_id_(0);

AudioSource::AudioSource(const glm::vec3 &position, const glm::vec3 &velocity,
                         const float pitch, const float gain, const bool loop,
                         const bool playing, const float obstructed)
    : position(position), velocity(velocity), pitch(pitch), gain(gain),
      loop(loop), playing(playing), obstructed(obstructed), id_(current_id_++) {
}

AudioSource::~AudioSource() {}

unsigned int AudioSource::id() const { return id_; }
}
