#pragma once

#include <atomic>
#include <string>
#include <memory>
#include <vector>

namespace mos::aud {

class Buffer;
using Shared_buffer = std::shared_ptr<Buffer>;

/** 16bit integer audio buffer. */
class Buffer final {
public:
  using Samples = std::vector<short>;

  template <class T>
  /** Construct buffer from a container of shorts. */
  Buffer(const T begin, const T end, const int channels = 1,
         const int sample_rate = 44100)
      : channels_(channels), sample_rate_(sample_rate), samples_(begin, end),
        id_(current_id_++) {}

  /** Empty buffer constructor. */
  explicit Buffer(int channels = 1, int sample_rate = 44100);

  /** Load shared buffer. */
  static auto load(const std::string &path) -> Buffer;

  Samples::const_iterator begin() const;

  Samples::const_iterator end() const;

  /** Raw data. */
  auto data() const -> const short *;

  /** Unique id. */
  auto id() const -> unsigned int;

  /** Get number of channels. */
  auto channels() const -> int;

  /** Get sample rate */
  auto sample_rate() const -> int;

  /** Duration in seconds. */
  auto duration() const -> float;

  /** Size of samples container. */
  auto size() const -> size_t;

private:
  static std::atomic_uint current_id_;
  unsigned int id_;
  Samples samples_;
  int channels_;
  int sample_rate_;
};
} // namespace mos::aud
