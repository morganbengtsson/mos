#pragma  once
#include <unordered_map>
#include <memory>
#include <thread>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx-presets.h>

#include <mos/aud/stream_speaker.hpp>
#include <mos/aud/speaker.hpp>
#include <mos/aud/listener.hpp>
#include <mos/aud/scene.hpp>

namespace mos {
namespace aud {

/** OpenAL audio system. */
class Renderer final {
public:
  Renderer();
  Renderer(const Renderer &renderer) = delete;
  Renderer(const Renderer &&renderer) = delete;
  Renderer & operator=(const Renderer & renderer) = delete;
  Renderer & operator=(const Renderer && renderer) = delete;
  ~Renderer();

  /** Get listener data. */
  Listener listener() const;

  /** Render and play audio scene. */
  void render(const Scene &scene);

  /** Clear buffers */
  void clear();

private:
  /** Set listener data */
  void listener(const Listener &listener);

  /** Update internal stream source representation. */
  void stream_source(const Stream_speaker &stream_source);

  /** Update internal buffer source representation. */
  void buffer_source(const Speaker &buffer_source);

  ALCdevice *device_;
  ALCcontext *context_;

  EFXEAXREVERBPROPERTIES reverb_properties;
  ALuint reverb_effect{0};
  ALuint reverb_slot{0};
  ALuint lowpass_filter1{0};
  ALuint lowpass_filter2{0};

  using SourcePair = std::pair<unsigned int, ALuint>;
  using BufferPair = std::pair<unsigned int, ALuint>;
  using Sources = std::unordered_map<unsigned int, ALuint>;
  using Buffers = std::unordered_map<unsigned int, ALuint>;
  using Filters = std::unordered_map<unsigned int, ALuint>;

  Sources sources_;
  Buffers buffers_;
  Filters filters_;

};
}
}
