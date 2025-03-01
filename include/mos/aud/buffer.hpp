#pragma once

#include <atomic>
#include <filesystem>
#include <memory>
#include <vector>

namespace mos::aud {

class Buffer;
using Shared_buffer = std::shared_ptr<Buffer>;

/** 16bit integer audio buffer. */
class Buffer final {
public:
  using Samples = std::vector<short>;

  static constexpr int default_num_channels = 1;
  static constexpr int default_sample_rate = 44100;

  template <class T>
  /** Construct buffer from a container of shorts. */
  Buffer(const T begin, const T end,
         int channels = default_num_channels,
         int sample_rate = default_sample_rate)
      : id_(current_id_++), samples_(begin, end), channels_(channels),
        sample_rate_(sample_rate) {}


  /** Empty buffer constructor. */
  explicit Buffer(int channels = default_num_channels, int sample_rate = default_sample_rate);

  /** Load shared buffer. */
  static auto load(const std::filesystem::path &path) -> Buffer;

  [[nodiscard]] auto begin() const -> Samples::const_iterator;

  [[nodiscard]] auto end() const -> Samples::const_iterator;

  /** Raw data. */
  [[nodiscard]] auto data() const -> const short *;

  /** Unique id. */
  [[nodiscard]] auto id() const -> unsigned int;

  /** Get number of channels. */
  [[nodiscard]] auto channels() const -> int;

  /** Get sample rate */
  [[nodiscard]] auto sample_rate() const -> int;

  /** Duration in seconds. */
  [[nodiscard]] auto duration() const -> float;

  /** Size of samples container. */
  [[nodiscard]] auto size() const -> size_t;

private:
  static std::atomic_uint current_id_;
  unsigned int id_;
  Samples samples_;
  int channels_;
  int sample_rate_;
};
} // namespace mos::aud
