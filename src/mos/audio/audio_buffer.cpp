#include <stdexcept>
#include <mos/audio/audio_buffer.hpp>
#include <stb_vorbis.h>
#include <fstream>

namespace mos {
std::atomic_uint AudioBuffer::current_id_;

AudioBuffer::AudioBuffer(const std::string &path) : id_(current_id_++) {
  short *decoded;

  std::ifstream file(path, std::ios::binary);
  if (!file.good()) {
    throw std::runtime_error(path + " does not exist.");
  }
  std::vector<unsigned char> data;

  unsigned char c;
  while (file.read(reinterpret_cast<char *>(&c), sizeof(c))) {
    data.push_back(c);
  }
  auto length = stb_vorbis_decode_memory(data.data(), data.size(), &channels_,
                                    &sample_rate_, &decoded);
  samples_.assign(decoded, decoded+length);
}

AudioBuffer::~AudioBuffer() {}

SharedAudioBuffer AudioBuffer::load(const std::string &path) {
  return std::make_shared<AudioBuffer>(path);
}

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
