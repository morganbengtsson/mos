#ifndef MOS_AUDIO_STREAM_SOURCE_HPP
#define MOS_AUDIO_STREAM_SOURCE_HPP

#include <glm/glm.hpp>
#include <memory>

#include <mos/audio/audio_stream.hpp>
#include <mos/audio/audio_source.hpp>

namespace mos {

/**
 * @brief The StreamSource class, for audio streaming
 *
 * Stream audio from an *ogg audio resource. Used with the Audio class for
 *playback.
 */
class AudioStreamSource {
public:
  /**
   * @brief StreamSource constructor.
   * @param stream
   * @param source
   */
  explicit AudioStreamSource(const std::shared_ptr<AudioStream> &stream,
                        const AudioSource &source = AudioSource());
  virtual ~AudioStreamSource();

  /**
   * @brief Stream used for the source.
   * @todo Should it be shared?
   */
  std::shared_ptr<AudioStream> stream;
  AudioSource source;
};
}

#endif // MOS_AUDIO_STREAM_SOURCE_HPP
