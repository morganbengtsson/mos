#pragma once
#include <array>
#include <string>
#include <atomic>
#include <stb_vorbis.h>
#include <memory>

namespace mos {
namespace aud {
class Stream;
using SharedStream = std::shared_ptr<Stream>;

/** Audio streaming from *.ogg file. */
class Stream final {
public:
  explicit Stream(const std::string &path);
  ~Stream();

  static const int buffer_size = 4096 * 8;

  std::array<short, buffer_size> read();

  bool done() const;

  int sample_rate() const;

  /** Get number of channels. */
  int channels() const;

  /** Duration in seconds. */
  float duration() const;

  /** Size of stream in samples. */
  size_t size() const;

  /** Restart streaming. */
  void seek_start();

  /** Unique id. */
  unsigned int id() const;
private:
  static std::atomic_uint current_id_;
  unsigned int id_;
  int samples_left_;
  int size_;
  std::string file_name_;
  stb_vorbis *vorbis_stream_;
  stb_vorbis_info vorbis_info_;
};
}
}
