#ifndef MOS_SOUND_H
#define MOS_SOUND_H

#include <vector>

namespace mos {

/**
 * A sound class containing samples of 16bit integer data. In memory.
 */
class Sound {
public:
  /**
   * @brief Container for 16bit samples (short).
   */
  using Samples = std::vector<short>;
  template <class It>

  /**
   * @brief Sound constructor.
   * @param begin iterator
   * @param end iterator
   * @param channels Number of channels
   *
   * Constructor that takes iterators from a container of shorts.
   */
  Sound(It begin, It end, const unsigned int channels = 1u,
        const unsigned int sample_rate = 44100u)
      : channels_(channels), sample_rate_(sample_rate), valid_(true),
        samples_(begin, end) {
    static unsigned int current_id = 0;
    id_ = current_id++;
  }

  /**
   * @brief Sound constructor empty
   * @param channels
   */
  Sound(const unsigned int channels = 1u) : channels_(channels) {
    static unsigned int current_id = 0;
    id_ = current_id++;
  }

  virtual ~Sound();

  /**
   * @brief begin iterator for samples
   * @return
   */
  Samples::const_iterator begin() const { return samples_.begin(); }

  /**
   * @brief end iterator for samples
   * @return
   */
  Samples::const_iterator end() const { return samples_.end(); }

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

private:
  unsigned int id_;
  Samples samples_;
  unsigned int channels_;
  unsigned int sample_rate_;
  bool valid_;
};
}

#endif /* MOS_SOUND_H */
