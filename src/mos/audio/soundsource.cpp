#include <mos/audio/soundsource.hpp>
#include <iostream>

namespace mos {

SoundSource::SoundSource(const std::shared_ptr<Sound> &sound,
                         const Source &source)
    : sound(sound), source(source), time_(.0f) {}

SoundSource::~SoundSource() {}

void SoundSource::update(const float dt) {
  if (source.playing) {
    time_ += dt;
  }  
  if (time_ > sound->length()){
    source.playing = false;
    time_ = .0f;
  }
}
}
