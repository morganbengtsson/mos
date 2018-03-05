#ifndef MOS_AUDIO_STREAM_SOURCE_HPP
#define MOS_AUDIO_STREAM_SOURCE_HPP

#include <glm/glm.hpp>
#include <memory>

#include <mos/audio/audio_types.hpp>
#include <mos/audio/audio_stream.hpp>
#include <mos/audio/audio_source.hpp>

namespace mos {

/**
 * @brief Stream audio from file, combined with source data.
 */
class AudioStreamSource final {
public:
  /**
   * @brief AudioStreamSource constructor.
   * @param stream
   * @param source
   */
  explicit AudioStreamSource(const std::shared_ptr<AudioStream> &stream,
                        const AudioSource &source = AudioSource());
  virtual ~AudioStreamSource();

  /**
   * @brief Stream used for the source.
   */
  std::shared_ptr<AudioStream> stream;

  /**
   * @brief source
   */
  AudioSource source;
};
}

#endif // MOS_AUDIO_STREAM_SOURCE_HPP
