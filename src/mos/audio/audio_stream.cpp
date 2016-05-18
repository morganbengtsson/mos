#include <mos/audio/audio_stream.hpp>

namespace mos {

std::atomic_uint AudioStream::current_id_(0);

AudioStream::AudioStream(const std::string & path) : file_name_(path) {
  vorbis_stream_ =
      stb_vorbis_open_filename((char *)path.c_str(), NULL, NULL);
  vorbis_info_ = stb_vorbis_get_info(vorbis_stream_);
  samples_left_ = stb_vorbis_stream_length_in_samples(vorbis_stream_) *
                  vorbis_info_.channels;

  id_ = current_id_++;
}

/*
AudioStream::Stream(const Stream &stream) : Stream(stream.file_name_) {
  id_ = current_id++;
}*/

AudioStream::~AudioStream() { stb_vorbis_close(vorbis_stream_); }

std::array<short, AudioStream::buffer_size> AudioStream::read() {
  auto samples = std::array<short, AudioStream::buffer_size>();

  int size = 0;
  int result = 0;

  while (size < AudioStream::buffer_size) {
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

bool AudioStream::done() const { return samples_left_ <= 0; }

int AudioStream::sample_rate() const { return vorbis_info_.sample_rate; }

int AudioStream::channels() const { return vorbis_info_.channels; }

void AudioStream::seek_start() {
  stb_vorbis_seek_start(vorbis_stream_);
  samples_left_ =
      stb_vorbis_stream_length_in_samples(vorbis_stream_) * channels();
}

unsigned int AudioStream::id() const { return id_; }
}
