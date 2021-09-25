#pragma  once

#include <unordered_map>
#include <memory>
#include <thread>

#include <mos/al/buffer.hpp>
#include <mos/al/source.hpp>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx-presets.h>

namespace mos::aud {
  class Sound_stream;
  class Sound;
  class Listener;
  class Scene;
}

namespace mos::al {

/** OpenAL audio renderer. */
class Renderer final {
public:
  Renderer();
  ~Renderer();

  Renderer(const Renderer &renderer) = delete;
  Renderer(const Renderer &&renderer) = delete;

  Renderer & operator=(const Renderer & renderer) = delete;
  Renderer & operator=(const Renderer && renderer) = delete;

  /** Get listener data. */
  auto listener() const -> aud::Listener;

  /** Render and play audio scene. */
  auto render(const aud::Scene &scene, const float dt) -> void;

  /** Clear buffers */
  auto clear() -> void;

private:
  /** Set listener data */
  auto listener(const aud::Listener &listener) -> void;

  /** Render/play a sound stream. */
  auto render_sound_stream(const aud::Sound_stream &sound_stream, const float dt) -> void;

  /** Render/play a sound. */
  auto render_sound(const aud::Sound &sound, const float dt) -> void;

  ALCdevice *device_;
  ALCcontext *context_;

  EFXEAXREVERBPROPERTIES reverb_properties;
  ALuint reverb_effect{0};
  ALuint reverb_slot{0};
  ALuint lowpass_filter1{0};
  ALuint lowpass_filter2{0};

  using SourcePair = std::pair<unsigned int, ALuint>;
  using BufferPair = std::pair<unsigned int, ALuint>;
  using Sources = std::unordered_map<unsigned int, Source>;
  using Buffers = std::unordered_map<unsigned int, Buffer>;
  using Filters = std::unordered_map<unsigned int, ALuint>;

  Sources sources_;
  Buffers buffers_;
  Filters filters_;
};
}
