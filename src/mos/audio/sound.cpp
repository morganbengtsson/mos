#include <mos/audio/sound.hpp>
#include <stb_vorbis.h>
#include <fstream>

namespace mos {
  unsigned int Sound::current_id_ = 0;

Sound::Sound(const std::string &path) {
  int channels, length, sample_rate;
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
  length = stb_vorbis_decode_memory(data.data(), data.size(), &channels,
                                    &sample_rate, &decoded);

  samples_.assign(decoded, decoded + length);
  channels_ = channels;
  sample_rate_ = sample_rate;
  id_ = current_id_++;
}

Sound::~Sound() {}

Sound::Samples::const_iterator Sound::begin() const { return samples_.begin(); }

Sound::Samples::const_iterator Sound::end() const { return samples_.end(); }

unsigned int Sound::id() const { return id_; }

bool Sound::valid() const { return valid_; }

unsigned int Sound::channels() const { return channels_; }

unsigned int Sound::sample_rate() const { return sample_rate_; }

float Sound::length() const {
  return float(samples_.size()) / float(sample_rate() * channels());
}

const short *Sound::data() const { return samples_.data(); }
}
