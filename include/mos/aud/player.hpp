#pragma  once
#include <unordered_map>
#include <memory>
#include <thread>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx-presets.h>

#include <mos/aud/stream_source.hpp>
#include <mos/aud/buffer_source.hpp>
#include <mos/aud/listener.hpp>
#include <mos/aud/scene.hpp>

namespace mos {
namespace aud {

/** Audio system. Uses OpenAL for Windows/Linux/OSX. */
class Player final {
public:
  Player();

  Player(const Player &audio) = delete;

  ~Player();

  /** Get listener data. */
  Listener listener() const;

  /** Render and play audio scene. */
  void play_scene(const Scene &batch);

  /** Clear buffers */
  void clear();

private:
  /** Set listener data */
  void listener(const Listener &listener);

  /** Update internal stream source representation. */
  void stream_source(const StreamSource &stream_source);

  /** Update internal buffer source representation. */
  void buffer_source(const BufferSource &buffer_source);


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
}