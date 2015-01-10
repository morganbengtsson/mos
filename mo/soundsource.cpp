#include "soundsource.hpp"

namespace mo {

SoundSource::SoundSource(const std::shared_ptr<Sound> sound, const glm::vec3 position, const bool loop) :
    sound(sound),
    position(position),
    loop(loop){
    static unsigned int current_id = 0;
    id_ = current_id++;
}

SoundSource::~SoundSource() {
}

unsigned int SoundSource::id() const{
    return id_;
}
}
