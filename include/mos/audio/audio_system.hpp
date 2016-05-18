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
class AudioSystem {
public:
  /**
   * @brief Default audio system constructor.
   */
  AudioSystem();

  /**
   * @brief Audio class is non copyable.
   * @param audio
   */
  AudioSystem(const AudioSystem &audio) = delete; // Do not copy
  virtual ~AudioSystem();

  template <class T>
  /**
   * A generalized load method. For sound and stream sources.
   *
   * @brief load
   * @param begin
   * @param end
   */
  void load(T begin, T end) {
    for (auto it = begin; it != end; it++) {
      load(*it);
    }
  }

  /**
   * Load a SoundSource object. A sound source contains one sound.
   *
   * @param source
   */
  void load(const AudioBufferSource &audio_buffer_source);

  bool loaded(const AudioBufferSource &audio_buffer_source);

  /**
   * Plays audio from a StreamSource. Which streams content from a file. This
   * method starts a new thread for each source playing.
   *
   * @brief load
   * @param stream_source
   */
  void load(const AudioStreamSource &stream_source);

  /**
   * @brief unload
   * @param sound_source
   */
  void unload(const AudioBufferSource &buffer_source);

  /**
   * @brief unload
   * @param stream_source
   */
  void unload(const AudioStreamSource &stream_source);

  template <class T>
  /**
   * @brief A generalized update method
   * @param begin Begin iterator.
   * @param end End iterator.
   * @param dt Time step.
   */
  void update(T begin, T end, const float dt) {
    for (auto it = begin; it != end; it++) {
      update(*it, dt);
    }
  }

  /**
   * Updates the internal source representation with data. Data
   * such as position, velocity, pitch and gain.
   *
   * @brief update
   * @param sound_source
   * @param dt Time step.
   */
  void update(const AudioBufferSource &buffer_source, const float dt);

  /**
  * Updates the internal source representation with data. Data
  * such as position, velocity, pitch and gain.
  *
  * @brief update
  * @param stream_source
* @param dt Time step.
  */
  void update(const AudioStreamSource &stream_source, const float dt);

  /**
   * @brief Get listener data.
   * @return listener
   */
  AudioListener listener() const;

  /**
   * @brief Set listener data
   * @param listener
   */
  void listener(const AudioListener &listener);

  void batch(const AudioBatch &batch);

private:
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
