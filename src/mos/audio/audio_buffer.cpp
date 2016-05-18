#include <fstream>
#include <mos/audio/audio_buffer.hpp>
#include <stb_vorbis.h>
#include <stdexcept>

namespace mos {
std::atomic_uint AudioBuffer::current_id_(0);

AudioBuffer::~AudioBuffer() {}

AudioBuffer::Samples::const_iterator AudioBuffer::begin() const {
  return samples_.begin();
}

AudioBuffer::Samples::const_iterator AudioBuffer::end() const {
  return samples_.end();
}

unsigned int AudioBuffer::id() const { return id_; }

bool AudioBuffer::valid() const { return valid_; }

unsigned int AudioBuffer::channels() const { return channels_; }

unsigned int AudioBuffer::sample_rate() const { return sample_rate_; }

float AudioBuffer::length() const {
  return float(samples_.size()) / float(sample_rate() * channels());
}

const short *AudioBuffer::data() const { return samples_.data(); }
}
