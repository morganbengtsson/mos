#include "soundsource.hpp"

namespace mo {

SoundSource::SoundSource(const std::shared_ptr<Sound> sound,
                         const glm::vec3 position,
                         const glm::vec3 velocity,
                         const float gain,
                         const float pitch,
                         const bool loop,
                         const bool playing,
                         const bool occluded):
    Source(position, velocity, pitch, gain, loop, playing, occluded),
    sound(sound){
}

SoundSource::~SoundSource() {
}

}
