#include "soundsource.hpp"

namespace mo {

SoundSource::SoundSource(const std::shared_ptr<Sound> sound,
                         const glm::vec3 position,
                         const glm::vec3 velocity,
                         const float gain,
                         const float pitch):
    Source(position, velocity, pitch, gain),
    sound(sound){
}

SoundSource::SoundSource(const SoundSource &sound_source) : Source(sound_source){
    sound = sound_source.sound;
}

SoundSource::~SoundSource() {
}

}
