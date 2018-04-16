#pragma once

#include <atomic>
#include <string>
#include <vector>
#include <memory>
#include <mos/aud/types.hpp>

namespace mos {
namespace aud {

/** Audio 16bit integer buffer. */
class Buffer final {
  friend class Renderer;
public:
  using Samples = std::vector<short>;

  template<class T>
  /** Construct buffer from a container of shorts. */
  Buffer(const T begin, const T end, const unsigned int channels = 1u,
              const unsigned int sample_rate = 44100u)
      : channels_(channels), sample_rate_(sample_rate), valid_(true),
        samples_(begin, end), id_(current_id_++) {}

  /** Empty buffer constructor. */
  Buffer(const unsigned int channels = 1u)
      : channels_(channels), id_(current_id_++) {}

  /** Construct from *.ogg file. */
  Buffer(const std::string &path);

  ~Buffer();

  /** Load shared buffer. */
  static SharedBuffer load(const std::string &path);

  Samples::const_iterator begin() const;

  Samples::const_iterator end() const;

  /** Raw data. */
  const short *data() const;

  /** Unique id. */
  unsigned int id() const;

  bool valid() const;

  //TODO: Deprecate and use modified time instead.
  void invalidate();

  /** Get number of channels. */
  unsigned int channels() const;

  /** Get sample rate */
  unsigned int sample_rate() const;

  /** Length in seconds. */
  float length() const;

private:
  static std::atomic_uint current_id_;
  unsigned int id_;
  Samples samples_;
  int channels_;
  int sample_rate_;
  //TODO: replace with modified time_point.
  bool valid_;
};
}
}