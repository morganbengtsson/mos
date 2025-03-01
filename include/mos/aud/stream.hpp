#pragma once

#include <array>
#include <memory>
#include <stb_vorbis.h>
#include <string>

#include <mos/core/id.hpp>

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
  auto operator=(const Stream &stream) -> Stream & = default;
  auto operator=(Stream &&stream) -> Stream & = default;

  static const int buffer_size = 4096 * 8;

  /** Read a chunk of data from the stream. */
  auto read() -> std::array<short, buffer_size>;

  /** Check if stream reached the end. */
  [[nodiscard]] auto done() const -> bool;

  /** Sample rate, eg 44100 */
  [[nodiscard]] auto sample_rate() const -> unsigned int;

  /** Get number of channels. */
  [[nodiscard]] auto channels() const -> unsigned int;

  /** Duration in seconds. */
  [[nodiscard]] auto duration() const -> float;

  /** Size of stream in samples. */
  [[nodiscard]] auto size() const -> size_t;

  /** Restart streaming. */
  auto seek_start() -> void;

  /** Unique id. */
  [[nodiscard]] auto id() const -> unsigned int;

private:
  Id<Stream> id_;
  unsigned int samples_left_;
  std::string file_name_;
  stb_vorbis *vorbis_stream_;
  stb_vorbis_info vorbis_info_;
};
} // namespace mos::aud
