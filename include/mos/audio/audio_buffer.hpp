#ifndef MOS_SOUND_H
#define MOS_SOUND_H

#include <vector>
#include <string>

namespace mos {

/**
 * A sound class containing samples of 16bit integer data. In memory.
 */
class AudioBuffer {
  friend class Player;
public:
  /**
   * @brief Sound constructor from *.ogg file.
   * @param path
   */
  AudioBuffer(const std::string &path);

  /**
   * @brief Container for 16bit samples (short).
   */
  using Samples = std::vector<short>;

  template <class T>
  /**
   * @brief Sound constructor.
   * @param begin iterator
   * @param end iterator
   * @param channels Number of channels
   *
   * Constructor that takes iterators from a container of shorts.
   */
  AudioBuffer(T begin, T end, const unsigned int channels = 1u,
        const unsigned int sample_rate = 44100u)
      : channels_(channels), sample_rate_(sample_rate), valid_(true),
        samples_(begin, end) {
    id_ = current_id_++;
  }

  /**
   * @brief Sound constructor empty
   * @param channels
   */
  AudioBuffer(const unsigned int channels = 1u) : channels_(channels) {
    id_ = current_id_++;
  }

  virtual ~AudioBuffer();

  /**
   * @brief begin iterator for samples
   * @return
   */
  Samples::const_iterator begin() const;

  /**
   * @brief end iterator for samples
   * @return
   */
  Samples::const_iterator end() const;

  /**
   * @brief Raw data
   * @return array of shorts
   */
  const short *data() const;

  /**
   * @brief unique id
   * @return id
   */
  unsigned int id() const;

  /**
   * @brief valid
   *
   * A sound becomes invalid, if its data has changed. For subsystems to know,
   *when to update.
   */
  bool valid() const;

  /**
   * @brief invalidate
   *
   * The sound has to be invalidated if data is changed. For subsystems to
   *update.
   */
  void invalidate();

  /**
   * @brief Number of channels
   * @return channels
   */
  unsigned int channels() const;

  /**
   * @brief Sample rate
   * @return sample rate
   */
  unsigned int sample_rate() const;

  /**
   * @brief length in seconds.
   * @return
   */
  float length() const;

private:
  static unsigned int current_id_;
  unsigned int id_;
  Samples samples_;
  unsigned int channels_;
  unsigned int sample_rate_;
  bool valid_;
};
}

#endif /* MOS_SOUND_H */
