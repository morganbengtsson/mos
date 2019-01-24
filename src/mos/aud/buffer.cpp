#include <stdexcept>
#include <mos/aud/buffer.hpp>
#include <stb_vorbis.h>
#include <fstream>

namespace mos {
namespace aud {

std::atomic_uint Buffer::current_id_;

Buffer::Buffer(const int channels, const int sample_rate) : channels_(channels), id_(current_id_++), sample_rate_(sample_rate) {}

Buffer::Buffer(const std::string &path) : id_(current_id_++), channels_(0), sample_rate_(0) {
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
  samples_.assign(decoded, decoded + length);
}

Shared_buffer Buffer::load(const std::string &path) {
  return std::make_shared<Buffer>(path);
}

Buffer::Samples::const_iterator Buffer::begin() const {
  return samples_.begin();
}

Buffer::Samples::const_iterator Buffer::end() const {
  return samples_.end();
}

unsigned int Buffer::id() const { return id_; }

int Buffer::channels() const { return channels_; }

int Buffer::sample_rate() const { return sample_rate_; }

float Buffer::duration() const {
  return float(samples_.size()) / float(sample_rate() * channels());
}

const short *Buffer::data() const { return samples_.data(); }

size_t Buffer::size() const {
  return samples_.size();
}

}
}
