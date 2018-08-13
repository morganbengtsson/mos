#pragma once

#include <atomic>
#include <string>
#include <vector>
#include <memory>

namespace mos {
namespace aud {

class Buffer;
using SharedBuffer = std::shared_ptr<Buffer>;

/** Audio 16bit integer buffer. */
class Buffer final {
  friend class Renderer;
public:
  //TODO: Replace with tracked container
  using Samples = std::vector<short>;

  template<class T>
  /** Construct buffer from a container of shorts. */
  Buffer(const T begin, const T end, const int channels = 1,
              const unsigned int sample_rate = 44100u)
      : channels_(channels), sample_rate_(sample_rate),
        samples_(begin, end), id_(current_id_++) {}

  /** Empty buffer constructor. */
  Buffer(const int channels = 1);

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

  /** Get number of channels. */
  int channels() const;

  /** Get sample rate */
  int sample_rate() const;

  /** Length in seconds. */
  float length() const;

private:
  static std::atomic_uint current_id_;
  unsigned int id_;
  Samples samples_;
  int channels_;
  int sample_rate_;
};
}
}