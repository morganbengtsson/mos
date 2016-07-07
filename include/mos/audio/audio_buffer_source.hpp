#ifndef MOS_AUDIO_BUFFER_SOURCE_HPP
#define MOS_AUDIO_BUFFER_SOURCE_HPP

#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include <mos/audio/audio_types.hpp>
#include <mos/audio/audio_buffer.hpp>
#include <mos/audio/audio_source.hpp>

namespace mos {

/**
 * Audio buffer and audio source combined.
 */
class AudioBufferSource final {
public:
  /**
   * @brief AudioBufferSource constructor.
   * @param buffer
   * @param source
   */
  AudioBufferSource(const SharedAudioBuffer &buffer = nullptr,
                       const AudioSource &source = AudioSource());
  ~AudioBufferSource();

  /**
   * @brief update the source and stop if not looping.
   * @param dt
   */
  void update(const float dt);

  SharedAudioBuffer buffer;
  AudioSource source;
private:
  float time_;
};
}

#endif // MOS_AUDIO_BUFFER_SOURCE_HPP
