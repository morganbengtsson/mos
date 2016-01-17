#include <mos/audio/soundsource.hpp>

namespace mos {

SoundSource::SoundSource(const std::shared_ptr<Sound> & sound,
                         const Source & source):
    sound(sound), source(source) {
}

SoundSource::~SoundSource() {
}

}
