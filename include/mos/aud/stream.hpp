#pragma once
#include <array>
#include <string>
#include <atomic>
#include <stb_vorbis.h>
#include <memory>

namespace mos::aud {

class Stream;
using Shared_stream = std::shared_ptr<Stream>;

/** Audio streaming from *.ogg file. */
class Stream final {
public:
  explicit Stream(const std::string &path);
  ~Stream();
  Stream(const Stream &stream) = default;
  Stream(Stream &&stream) = default;
  Stream & operator=(const Stream &stream) = default;
  Stream & operator=(Stream &&stream) = default;

  static const int buffer_size = 4096 * 8;

  auto read() -> std::array<short, buffer_size>;

  auto done() const -> bool;

  auto sample_rate() const -> int;

  /** Get number of channels. */
  auto channels() const -> int;

  /** Duration in seconds. */
  auto duration() const -> float;

  /** Size of stream in samples. */
  auto size() const -> size_t;

  /** Restart streaming. */
  auto seek_start() -> void;

  /** Unique id. */
  auto id() const -> unsigned int;
private:
  static std::atomic_uint current_id_;
  unsigned int id_;
  int samples_left_;
  std::string file_name_;
  stb_vorbis *vorbis_stream_;
  stb_vorbis_info vorbis_info_;
};
}
