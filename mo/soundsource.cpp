#include "soundsource.hpp"

namespace mo {

SoundSource::SoundSource(const std::shared_ptr<Sound> sound,
                         const glm::vec3 position) :
    Source(position),
    sound(sound){
}

SoundSource::~SoundSource() {
}

}
