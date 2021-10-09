#include <mos/apu/buffer.hpp>

auto mos::apu::Buffer::id() const -> unsigned int { return id_; }

auto mos::apu::Buffer::channels() const -> int { return channels_; }

auto mos::apu::Buffer::sample_rate() const -> int { return sample_rate_; }

auto mos::apu::Buffer::duration() const -> float {
  return float(size_) / float(sample_rate() * channels());
}

auto mos::apu::Buffer::size() const -> size_t { return size_; }

mos::apu::Buffer::Buffer(const aud::Buffer &buffer)
    : id_(buffer.id()), channels_(buffer.channels()),
      sample_rate_(buffer.sample_rate()), size_(buffer.size()) {}
