#ifndef MOS_PLAYER_H
#define MOS_PLAYER_H

#include <unordered_map>
#include <memory>
#include <thread>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx-presets.h>

#include <mos/audio/streamsource.hpp>
#include <mos/audio/soundsource.hpp>
#include <mos/audio/listener.hpp>

namespace mos {

/**
 * Audio player class. Uses OpenAL for Windows/Linux/OSX.
 */
class Player {
public:
  /**
   * @brief Default audio player constructor.
   */
  Player();

  /**
   * @brief Audio class is non copyable.
   * @param audio
   */
  Player(const Player &audio) = delete; // Do not copy
  virtual ~Player();

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
  void load(const SoundSource &sound_source);

  bool loaded(const SoundSource &sound_source);

  /**
   * Plays audio from a StreamSource. Which streams content from a file. This
   * method starts a new thread for each source playing.
   *
   * @brief load
   * @param stream_source
   */
  void load(const StreamSource &stream_source);

  /**
   * @brief unload
   * @param sound_source
   */
  void unload(const SoundSource &sound_source);

  /**
   * @brief unload
   * @param stream_source
   */
  void unload(const StreamSource &stream_source);

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
  void update(const SoundSource &sound_source, const float dt);

  /**
  * Updates the internal source representation with data. Data
  * such as position, velocity, pitch and gain.
  *
  * @brief update
  * @param stream_source
* @param dt Time step.
  */
  void update(StreamSource &stream_source, const float dt);

  /**
   * @brief Get listener data.
   * @return listener
   */
  Listener listener() const;

  /**
   * @brief Set listener data
   * @param listener
   */
  void listener(const Listener &listener);

private:
  struct StreamThread {
    std::shared_ptr<std::thread> thread;
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

#endif /* MOS_PLAYER_H */
