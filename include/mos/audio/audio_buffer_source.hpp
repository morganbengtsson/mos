#ifndef MOS_AUDIO_BUFFER_SOURCE_HPP
#define MOS_AUDIO_BUFFER_SOURCE_HPP

#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include <mos/audio/audio_buffer.hpp>
#include <mos/audio/audio_source.hpp>

namespace mos {

/**
 * A classed used for audio playback. Contains sounds, together with a position.
 * That is used if 3D audio is enabled.
 */
class AudioBufferSource {
public:
  /**
   * @brief SoundSource constructor.
   * @param sound
   * @param source
   */
  explicit AudioBufferSource(const std::shared_ptr<AudioBuffer> &buffer,
                       const AudioSource &source = AudioSource());
  virtual ~AudioBufferSource();

  /**
   * @brief update the source and stop if not looping.
   * @param dt
   */
  void update(const float dt);

  /**
   * @brief The sound that is played from the source.
   */
  std::shared_ptr<AudioBuffer> buffer;
  AudioSource source;
private:
  float time_;
};
}

#endif // MOS_AUDIO_BUFFER_SOURCE_HPP
