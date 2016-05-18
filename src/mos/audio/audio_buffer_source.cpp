#include <mos/audio/audio_buffer_source.hpp>
#include <iostream>

namespace mos {

AudioBufferSource::AudioBufferSource(const std::shared_ptr<AudioBuffer> &buffer,
                         const AudioSource &source)
    : buffer(buffer), source(source), time_(.0f) {}

AudioBufferSource::~AudioBufferSource() {}

void AudioBufferSource::update(const float dt) {
  if (source.playing) {
    time_ += dt;
  }  
  if (time_ > buffer->length()){
    source.playing = false;
    time_ = .0f;
  }
}
}
