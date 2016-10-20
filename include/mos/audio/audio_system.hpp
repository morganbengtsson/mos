#ifndef MOS_AUDIO_SYTEM_H
#define MOS_AUDIO_SYSTEM_H

#include <unordered_map>
#include <memory>
#include <thread>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx-presets.h>

#include <mos/audio/audio_stream_source.hpp>
#include <mos/audio/audio_buffer_source.hpp>
#include <mos/audio/audio_listener.hpp>
#include <mos/audio/audio_batch.hpp>

namespace mos {

/**
 * @brief Audio system. Uses OpenAL for Windows/Linux/OSX.
 */
class AudioSystem final {
public:
  /**
   * @brief Default audio system constructor.
   */
  AudioSystem();

  /**
   * @brief Audio class is non copyable.
   * @param audio
   */
  AudioSystem(const AudioSystem &audio) = delete;
  ~AudioSystem();

  /**
   * @brief Get listener data.
   * @return listener
   */
  AudioListener listener() const;

  /**
   * @brief Update internal representation.
   * @param batch of audio related data.
   */
  void batch(const AudioBatch &batch);

  /**
   * @brief clear_buffers
   */
  void clear();

private:
  /**
   * @brief Set listener data
   * @param listener
   */
  void listener(const AudioListener &listener);

  /**
  * @brief Update internal stream source representation.
  * @param stream_source
  */
  void stream_source(const AudioStreamSource &stream_source);

  /**
   * @brief Update internal buffer source representation.
   * @param sound_source
   */
  void buffer_source(const AudioBufferSource &buffer_source);


  struct StreamThread {
    std::thread thread;
    bool running;
  };

  ALCdevice *device_;
  ALCcontext *context_;

  EFXEAXREVERBPROPERTIES reverb_properties;
  ALuint reverb_effect;
  ALuint reverb_slot;

  ALuint lowpass_filter1;
  ALuint lowpass_filter2;

  using SourcePair = std::pair<unsigned int, ALuint>;
  using BufferPair = std::pair<unsigned int, ALuint>;
  using Sources = std::unordered_map<unsigned int, ALuint>;
  using Buffers = std::unordered_map<unsigned int, ALuint>;
  using Filters = std::unordered_map<unsigned int, ALuint>;

  Sources sources_;
  Buffers buffers_;
  Filters filters_;

  std::unordered_map<unsigned int, StreamThread> stream_threads;
};
}

#endif /* MOS_AUDIO_SYSTEM_H */
