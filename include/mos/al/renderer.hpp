#pragma once

#include <unordered_map>
#include <utility>
#include <vector>

#include <mos/al/buffer.hpp>
#include <mos/al/filter.hpp>
#include <mos/al/source.hpp>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/efx-presets.h>

#include <mos/apu/scene.hpp>
#include <mos/apu/sound.hpp>
#include <mos/apu/sound_stream.hpp>
#include <mos/aud/sound_streams.hpp>
#include <mos/aud/sounds.hpp>

namespace mos::aud {
class Sound_stream;
class Sound;
class Listener;
} // namespace mos::aud

namespace mos::apu {
class Scene;
} // namespace mos::apu

namespace mos::al {

/** OpenAL audio renderer. */
class Renderer final {
public:
  Renderer();
  ~Renderer();

  Renderer(const Renderer &renderer) = delete;
  Renderer(const Renderer &&renderer) = delete;

  auto operator=(const Renderer &renderer) -> Renderer & = delete;
  auto operator=(const Renderer &&renderer) -> Renderer & = delete;

  /** Get listener data. */
  [[nodiscard]] static auto listener() -> aud::Listener;

  [[nodiscard]] auto load(const aud::Sounds &sounds) -> std::vector<mos::apu::Sound>;

  [[nodiscard]] auto load(const aud::Sound_streams &sound_streams)
      -> std::vector<mos::apu::Sound_stream>;

  /** Render and play audio scene. */
  auto render(const apu::Scene &scene, float delta_time) -> void;

  /** Clear buffers */
  auto clear() -> void;

private:
  /** Set listener data */
  static auto listener(const aud::Listener &listener) -> void;

  /** Render/play a sound stream. */
  auto render_sound_stream(const apu::Sound_stream &sound_stream,
                           float delta_time) -> void;

  /** Render/play a sound. */
  auto render_sound(const apu::Sound &sound, float delta_time) -> void;

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
  using Filters = std::unordered_map<unsigned int, Filter>;

  Sources sources_;
  Buffers buffers_;
  Filters filters_;
};
} // namespace mos::al
