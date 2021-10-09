#include <chrono>
#include <glm/gtx/io.hpp>
#include <iostream>
#include <thread>

#include <mos/al/renderer.hpp>

#include <mos/aud/sound_stream.hpp>
#include <mos/aud/sound.hpp>
#include <mos/aud/listener.hpp>
#include <mos/aud/scene.hpp>

namespace mos::al {

Renderer::Renderer()
    : reverb_properties(EFX_REVERB_PRESET_LIVINGROOM) {
  static constexpr ALint sampling_frequency{44100};
  std::array context_attributes{ALC_FREQUENCY, sampling_frequency, ALC_HRTF_SOFT, ALC_TRUE, 0};
  device_ = alcOpenDevice(nullptr);
  if (device_ == AL_NONE) {
    throw std::runtime_error("No valid audio device");
  }
  context_ = alcCreateContext(device_, context_attributes.data());
  alcMakeContextCurrent(context_);

  if (alcIsExtensionPresent(alcGetContextsDevice(alcGetCurrentContext()),
                            "ALC_EXT_EFX") == AL_NONE) {
    throw std::runtime_error("OpenAL EFX not supported.");
  }

  ALuint reverb_effect = AL_NONE;
  alGenEffects(1, &reverb_effect);
  alEffecti(reverb_effect, AL_EFFECT_TYPE, AL_EFFECT_REVERB);
  alEffectf(reverb_effect, AL_REVERB_DENSITY, reverb_properties.flDensity);
  alEffectf(reverb_effect, AL_REVERB_DIFFUSION, reverb_properties.flDiffusion);
  alEffectf(reverb_effect, AL_REVERB_GAIN, reverb_properties.flGain);
  alEffectf(reverb_effect, AL_REVERB_GAINHF, reverb_properties.flGainHF);
  alEffectf(reverb_effect, AL_REVERB_DECAY_TIME, reverb_properties.flDecayTime);
  alEffectf(reverb_effect, AL_REVERB_DECAY_HFRATIO,
            reverb_properties.flDecayHFRatio);
  alEffectf(reverb_effect, AL_REVERB_REFLECTIONS_GAIN,
            reverb_properties.flReflectionsGain);
  alEffectf(reverb_effect, AL_REVERB_REFLECTIONS_DELAY,
            reverb_properties.flReflectionsDelay);
  alEffectf(reverb_effect, AL_REVERB_LATE_REVERB_GAIN,
            reverb_properties.flLateReverbGain);
  alEffectf(reverb_effect, AL_REVERB_LATE_REVERB_DELAY,
            reverb_properties.flLateReverbDelay);
  alEffectf(reverb_effect, AL_REVERB_AIR_ABSORPTION_GAINHF,
            reverb_properties.flAirAbsorptionGainHF);
  alEffectf(reverb_effect, AL_REVERB_ROOM_ROLLOFF_FACTOR,
            reverb_properties.flRoomRolloffFactor);
  alEffecti(reverb_effect, AL_REVERB_DECAY_HFLIMIT,
            reverb_properties.iDecayHFLimit);

  if (reverb_effect == AL_NONE) {
    throw std::runtime_error("Could not create reverb effect.");
  }

  alGenAuxiliaryEffectSlots(1, &reverb_slot);
  if (reverb_slot == AL_NONE) {
    throw std::runtime_error("Could not create reverb effect slot.");
  }

  alAuxiliaryEffectSloti(reverb_slot, AL_EFFECTSLOT_EFFECT, reverb_effect);

  alGenFilters(1, &lowpass_filter1);
  if (lowpass_filter1 == AL_NONE) {
    std::runtime_error("Could not create lowpass filter.");
  }
  alFilteri(lowpass_filter1, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
  alFilterf(lowpass_filter1, AL_LOWPASS_GAIN, 0.6f);
  alFilterf(lowpass_filter1, AL_LOWPASS_GAINHF, 0.05f);

  alGenFilters(1, &lowpass_filter2);
  if (lowpass_filter2 == AL_NONE) {
    std::runtime_error("Could not create lowpass filter.");
  }
  alFilteri(lowpass_filter2, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
  alFilterf(lowpass_filter2, AL_LOWPASS_GAIN, 0.3f);
  alFilterf(lowpass_filter2, AL_LOWPASS_GAINHF, 0.01f);

  listener(aud::Listener());
}

Renderer::~Renderer() {
  for (auto &source : sources_) {
    source.second.stop();
  }
  alcMakeContextCurrent(nullptr);
  alcDestroyContext(context_);
  alcCloseDevice(device_);
}

void Renderer::render_sound(const apu::Sound &sound, const float dt) {

  auto &source = sources_.at(sound.source.id());

  source.update(sound.source);

  auto & filter = filters_.at(sound.source.id());

  filter.update(sound.source, dt);

  // TODO: Check if needed
  //alSourcef(source.id, AL_GAIN, gain * sound.source.gain);
  //alSourcei(source.id, AL_DIRECT_FILTER, al_filter);

  ALenum state{AL_STOPPED};
  alGetSourcei(source.id, AL_SOURCE_STATE, &state);

  if (sound.source.playing && (state != AL_PLAYING)) {
    alSourcePlay(source.id);
  }

  if (!sound.source.playing && (state == AL_PLAYING)) {
    alSourceStop(source.id);
  }

  if (state == AL_STOPPED) {
    alSourceRewind(source.id);
  }
}

/*
void Renderer::render_sound_stream(const aud::Sound_stream &sound_stream, const float dt) {
  if (sources_.find(sound_stream.source.id()) == sources_.end()) {
    sources_.insert({sound_stream.source.id(), Source(sound_stream.source)});

    const auto &source = sources_.at(sound_stream.source.id());

    alSource3i(source.id, AL_AUXILIARY_SEND_FILTER, reverb_slot, 0,
               AL_FILTER_NULL);

    filters_.insert({sound_stream.source.id(), Filter(sound_stream.source)});

    auto &filter = filters_.at(sound_stream.source.id());

    alFilteri(filter.id, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
    alSourcei(source.id, AL_DIRECT_FILTER, filter.id);
  };

  auto &source = sources_.at(sound_stream.source.id());
  source.update(sound_stream.source);

  auto & filter = filters_.at(sound_stream.source.id());
  filter.update(sound_stream.source, dt);

  // TODO: Check if needed
  //alSourcef(source.id, AL_GAIN, gain * sound.source.gain);
  //alSourcei(source.id, AL_DIRECT_FILTER, al_filter);

  ALenum state{AL_STOPPED};
  alGetSourcei(source.id, AL_SOURCE_STATE, &state);

  ALint processed = 0;
  alGetSourcei(source.id, AL_BUFFERS_PROCESSED, &processed);
  ALuint buffer = 0;

  if(sound_stream.stream) {
    auto format = sound_stream.stream->channels() == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

    std::array<ALuint,4> buffers{0,0,0,0};
    if (sound_stream.source.playing && (state != AL_PLAYING)) {
      alGenBuffers(4, buffers.data());
      const auto samples = sound_stream.stream->read();
      const int size = sound_stream.stream->buffer_size;
      for (const auto & buffer : buffers) {
        alBufferData(
            buffer, format, samples.data(),
            size * sizeof(ALshort), sound_stream.stream->sample_rate());
        alSourceQueueBuffers(source.id, 1, &buffer);
      }
      alSourcePlay(source.id);
    }

    while (processed--) {		
                  alSourceUnqueueBuffers(source.id, 1, &buffer);
                  const auto samples = sound_stream.stream->read();
                  const int size = sound_stream.stream->buffer_size;
			if (buffer != 0) {
	  			alBufferData(buffer, format, samples.data(),
					size * sizeof(ALshort),
					sound_stream.stream->sample_rate());
                                alSourceQueueBuffers(source.id, 1, &buffer);
			}
    }
    if (sound_stream.source.loop && sound_stream.stream->done()) {
      sound_stream.stream->seek_start();
    }
    if (!sound_stream.source.playing && (state == AL_PLAYING)) {
      alSourceStop(source.id);
      ALint count{0};
      alGetSourcei(source.id, AL_BUFFERS_QUEUED, &count);
      alSourceUnqueueBuffers(source.id, count, &buffer);
      alDeleteBuffers(4, buffers.data());
    }
  }
}
*/

auto Renderer::listener() -> aud::Listener {
  aud::Listener listener;
  alGetListener3f(AL_POSITION, &listener.position.x, &listener.position.y,
                  &listener.position.z);

  std::array<float,6> orientation{};
  alGetListenerf(AL_ORIENTATION, orientation.data());
  listener.direction.x = orientation[0];
  listener.direction.y = orientation[1];
  listener.direction.z = orientation[2];
  listener.up.x = orientation[3];
  listener.up.y = orientation[4];
  listener.up.z = orientation[5];

  alGetListener3f(AL_VELOCITY, &listener.velocity.x, &listener.velocity.y,
                  &listener.velocity.z);

  return listener;
}

std::vector<apu::Sound> Renderer::load(const aud::Sounds &sounds) {
  std::vector<apu::Sound> out_sounds;
  for (const auto &sound : sounds) {
    auto apu_sound = apu::Sound(sound);
    if (sources_.find(sound.source.id()) == sources_.end()) {
      sources_.insert({sound.source.id(), mos::al::Source(apu_sound.source)});

      const auto &source = sources_.at(sound.source.id());

      alSource3i(source.id, AL_AUXILIARY_SEND_FILTER, reverb_slot, 0,
                 AL_FILTER_NULL);

      filters_.insert({sound.source.id(), Filter(source)});
    }

    auto &source = sources_.at(sound.source.id());

    auto buffer = sound.buffer;
    if (buffer) {
      if (buffers_.find(buffer->id()) == buffers_.end()) {
        buffers_.insert({buffer->id(), al::Buffer(*buffer)});
      }

      ALuint al_buffer = buffers_.at(sound.buffer->id()).id;

      int v{0u};
      alGetSourcei(source.id, AL_BUFFER, &v);
      if (v == 0) {
        alSourcei(source.id, AL_BUFFER, al_buffer);
      }
    }
    out_sounds.push_back(apu_sound);
  }
  return out_sounds;
}

auto Renderer::listener(const aud::Listener &listener) -> void {
  alListener3f(AL_POSITION, listener.position.x, listener.position.y,
               listener.position.z);

  alListener3f(AL_VELOCITY, listener.velocity.x, listener.velocity.y,
               listener.velocity.z);

  std::array<float, 6> orientation {listener.direction.x, listener.direction.y,
                          listener.direction.z, listener.up.x,
                          listener.up.y, listener.up.z};
  alListenerfv(AL_ORIENTATION, orientation.data());
  alListenerf(AL_GAIN, listener.gain);
}

auto Renderer::render(const apu::Scene &scene, const float dt) -> void {
  listener(scene.listener);
  for (const auto &sound : scene.sounds) {
    render_sound(sound, dt);
  }
  /*
  for (const auto &sound_stream : scene.sound_streams) {
    render_sound_stream(sound_stream, dt);
  }*/
}

void Renderer::clear() {
  for (auto& source : sources_) {
    source.second.stop();
  }

  sources_.clear();
  buffers_.clear();
}
}
