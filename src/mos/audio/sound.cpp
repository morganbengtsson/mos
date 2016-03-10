#include <mos/audio/sound.hpp>

namespace mos {
  Sound::Sound(const std::string & path) {

  }

  Sound::~Sound() {}

  Sound::Samples::const_iterator Sound::begin() const { return samples_.begin(); }

  Sound::Samples::const_iterator Sound::end() const { return samples_.end(); }

  unsigned int Sound::id() const { return id_; }

  bool Sound::valid() const { return valid_; }

  unsigned int Sound::channels() const { return channels_; }

  unsigned int Sound::sample_rate() const { return sample_rate_; }

  const short *Sound::data() const { return samples_.data(); }
}
