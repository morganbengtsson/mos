#include <mos/audio/stream.hpp>

namespace mos {

Stream::Stream(const std::string & file_name) : file_name_(file_name) {
  vorbis_stream_ =
      stb_vorbis_open_filename((char *)file_name.c_str(), NULL, NULL);
  vorbis_info_ = stb_vorbis_get_info(vorbis_stream_);
  samples_left_ = stb_vorbis_stream_length_in_samples(vorbis_stream_) *
                  vorbis_info_.channels;
  static unsigned int current_id = 0;
  id_ = current_id++;
}

Stream::Stream(const Stream &stream) : Stream(stream.file_name_) {
  static unsigned int current_id = 0;
  id_ = current_id++;
}

Stream::~Stream() { stb_vorbis_close(vorbis_stream_); }

std::array<short, Stream::buffer_size> Stream::read() {
  auto samples = std::array<short, Stream::buffer_size>();

  int size = 0;
  int result = 0;

  while (size < Stream::buffer_size) {
    result = stb_vorbis_get_samples_short_interleaved(
        vorbis_stream_, vorbis_info_.channels, samples.data() + size,
        buffer_size - size);
    if (result > 0)
      size += result * vorbis_info_.channels;
    else
      break;
  }
  samples_left_ -= size;
  return samples;
}

bool Stream::done() const { return samples_left_ <= 0; }

int Stream::sample_rate() const { return vorbis_info_.sample_rate; }

int Stream::channels() const { return vorbis_info_.channels; }

void Stream::seek_start() {
  stb_vorbis_seek_start(vorbis_stream_);
  samples_left_ =
      stb_vorbis_stream_length_in_samples(vorbis_stream_) * channels();
}

unsigned int Stream::id() const { return id_; }
}
