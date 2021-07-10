#include <mos/aud/stream.hpp>

namespace mos::aud {

std::atomic_uint Stream::current_id_;

Stream::Stream(const std::string &path)
    : id_(current_id_++), file_name_(path), vorbis_info_() {
  vorbis_stream_ = stb_vorbis_open_filename(path.c_str(), nullptr, nullptr);
  vorbis_info_ = stb_vorbis_get_info(vorbis_stream_);
  samples_left_ = stb_vorbis_stream_length_in_samples(vorbis_stream_) *
      vorbis_info_.channels;
}

Stream::~Stream() { stb_vorbis_close(vorbis_stream_); }

auto Stream::read() -> std::array<short, buffer_size> {
  auto samples = std::array<short, buffer_size>();

  int size = 0;
  int result = 0;

  while (size < buffer_size) {
    result = stb_vorbis_get_samples_short_interleaved(
        vorbis_stream_, vorbis_info_.channels, samples.data() + size,
        buffer_size - size);
    if (result > 0) {
      size += result * vorbis_info_.channels;
    } else {
      break;
    }
  }
  samples_left_ -= size;
  return samples;
}

auto Stream::done() const -> bool { return samples_left_ <= 0; }

auto Stream::sample_rate() const -> unsigned int { return vorbis_info_.sample_rate; }

auto Stream::channels() const -> unsigned int { return vorbis_info_.channels; }

void Stream::seek_start() {
  stb_vorbis_seek_start(vorbis_stream_);
  samples_left_ = size() * channels();
}

auto Stream::id() const -> unsigned int { return id_; }

auto Stream::duration() const -> float {
  return stb_vorbis_stream_length_in_seconds(vorbis_stream_);
}
auto Stream::size() const -> size_t {
  return stb_vorbis_stream_length_in_samples(vorbis_stream_);
}
}
