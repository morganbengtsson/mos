#include <stdexcept>
#include <mos/aud/buffer.hpp>
#include <stb_vorbis.h>
#include <fstream>

namespace mos::aud {

std::atomic_uint Buffer::current_id_;

Buffer::Buffer(const int channels, const int sample_rate) : id_(current_id_++), channels_(channels), sample_rate_(sample_rate) {}

Buffer::Buffer(const std::string &path) : id_(current_id_++), channels_(0), sample_rate_(0) {
  short *decoded{};

  std::ifstream file(path, std::ios::binary);
  if (!file.good()) {
    throw std::runtime_error(path + " does not exist.");
  }
  std::vector<unsigned char> data(std::istreambuf_iterator<char>(file), {});

  auto length = stb_vorbis_decode_memory(data.data(), data.size(), &channels_,
                                         &sample_rate_, &decoded);
  samples_.assign(decoded, decoded + length);
}

auto Buffer::load(const std::string &path) -> Shared_buffer {
  return std::make_shared<Buffer>(path);
}

auto Buffer::begin() const -> Samples::const_iterator {
  return samples_.begin();
}

auto Buffer::end() const -> Samples::const_iterator {
  return samples_.end();
}

auto Buffer::id() const -> unsigned int { return id_; }

auto Buffer::channels() const -> int { return channels_; }

auto Buffer::sample_rate() const -> int { return sample_rate_; }

auto Buffer::duration() const -> float {
  return float(samples_.size()) / float(sample_rate() * channels());
}

auto Buffer::data() const -> const short * { return samples_.data(); }

auto Buffer::size() const -> size_t {
  return samples_.size();
}

}
