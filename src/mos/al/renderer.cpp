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

  if (!alcIsExtensionPresent(alcGetContextsDevice(alcGetCurrentContext()),
                             "ALC_EXT_EFX")) {
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
  for (auto source : sources_) {
    alSourceStop(source.second);
    alDeleteSources(1, &source.second);
  }
  for (auto buffer : buffers_) {
    alDeleteBuffers(1, &buffer.second);
  }
  alcMakeContextCurrent(nullptr);
  alcDestroyContext(context_);
  alcCloseDevice(device_);
}

void Renderer::render_sound(const aud::Sound &sound, const float dt) {
  if (sources_.find(sound.source.id()) == sources_.end()) {
    ALuint al_source{AL_NONE};
    alGenSources(1, &al_source);
    sources_.insert(SourcePair(sound.source.id(), al_source));

    alSource3i(al_source, AL_AUXILIARY_SEND_FILTER, reverb_slot, 0,
               AL_FILTER_NULL);
    ALuint al_filter{AL_NONE};
    alGenFilters(1, &al_filter);
    filters_.insert(SourcePair(sound.source.id(), al_filter));
    alFilteri(al_filter, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
    alSourcei(al_source, AL_DIRECT_FILTER, al_filter);
  }

  ALuint al_source = sources_.at(sound.source.id());

  auto buffer = sound.buffer;
  if (buffer) {
    auto format =
        buffer->channels() == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
    if (buffers_.find(buffer->id()) == buffers_.end()) {
        ALuint al_buffer{0u};
      alGenBuffers(1, &al_buffer);
      {
        long data_size = std::distance(buffer->begin(), buffer->end());
        const ALvoid *data = buffer->data();
        alBufferData(al_buffer, format, data, data_size * sizeof(short),
                     buffer->sample_rate());
      }
      buffers_.insert(BufferPair(buffer->id(), al_buffer));
    }

    ALuint al_buffer = buffers_.at(sound.buffer->id());

    int v{0u};
    alGetSourcei(al_source, AL_BUFFER, &v);
    if (v == 0) {
      alSourcei(al_source, AL_BUFFER, al_buffer);
    }
  }

  alSourcei(al_source, AL_LOOPING, sound.source.loop);
  alSourcef(al_source, AL_PITCH, sound.source.pitch);
  //alSourcef(al_source, AL_GAIN, buffer_source.source.gain);
  alSource3f(al_source, AL_POSITION, sound.source.position.x,
             sound.source.position.y, sound.source.position.z);
  alSource3f(al_source, AL_VELOCITY, sound.source.velocity.x,
             sound.source.velocity.y, sound.source.velocity.z);

  auto al_filter = filters_[sound.source.id()];
  ALfloat al_gain{0.0f};
  alGetFilterf(al_filter, AL_LOWPASS_GAIN, &al_gain);
  auto error = glm::clamp((1.0f - sound.source.obstructed), 0.0f, 1.0f) - al_gain;
  float gain = al_gain + error * dt;

  ALfloat al_gain_hf{0.0f};
  alGetFilterf(al_filter, AL_LOWPASS_GAINHF, &al_gain_hf);
  auto error_hf = glm::clamp((1.0f - sound.source.obstructed), 0.0f, 1.0f) - al_gain_hf;
  float gain_hf = al_gain_hf + error_hf * dt;

  alSourcef(al_source, AL_GAIN, gain * sound.source.gain);

  alFilteri(al_filter, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
  alFilterf(al_filter, AL_LOWPASS_GAIN, gain);      // 0.5f
  alFilterf(al_filter, AL_LOWPASS_GAINHF, gain_hf); // 0.01f
  alSourcei(al_source, AL_DIRECT_FILTER, al_filter);

  ALenum state{AL_STOPPED};
  alGetSourcei(al_source, AL_SOURCE_STATE, &state);

  if (sound.source.playing && (state != AL_PLAYING)) {
    alSourcePlay(al_source);
  }

  if (!sound.source.playing && (state == AL_PLAYING)) {
    alSourceStop(al_source);
  }

  if (state == AL_STOPPED) {
    alSourceRewind(al_source);
  }
}

void Renderer::render_sound_stream(const aud::Sound_stream &sound_stream, const float dt) {
  if (sources_.find(sound_stream.source.id()) == sources_.end()) {
    ALuint al_source{0u};
    alGenSources(1, &al_source);
    sources_.insert(SourcePair(sound_stream.source.id(), al_source));

    alSource3i(al_source, AL_AUXILIARY_SEND_FILTER, reverb_slot, 0,
               AL_FILTER_NULL);
    ALuint al_filter{0u};
    alGenFilters(1, &al_filter);
    filters_.insert(SourcePair(sound_stream.source.id(), al_filter));
    alFilteri(al_filter, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
    alSourcei(al_source, AL_DIRECT_FILTER, al_filter);
  };

  ALuint al_source = sources_.at(sound_stream.source.id());
  alSourcei(al_source, AL_LOOPING, sound_stream.source.loop);
  alSourcef(al_source, AL_PITCH, sound_stream.source.pitch);
  //alSourcef(al_source, AL_GAIN, stream_source.source.gain);
  alSource3f(al_source, AL_POSITION, sound_stream.source.position.x,
             sound_stream.source.position.y, sound_stream.source.position.z);
  alSource3f(al_source, AL_VELOCITY, sound_stream.source.velocity.x,
             sound_stream.source.velocity.y, sound_stream.source.velocity.z);

  auto al_filter = filters_[sound_stream.source.id()];
  ALfloat al_gain{0.0f};
  alGetFilterf(al_filter, AL_LOWPASS_GAIN, &al_gain);
  auto error = glm::clamp((1.0f - sound_stream.source.obstructed), 0.0f, 1.0f) - al_gain;
  float gain = al_gain + error * dt;

  ALfloat al_gain_hf{0.0f};
  alGetFilterf(al_filter, AL_LOWPASS_GAINHF, &al_gain_hf);
  auto error_hf = glm::clamp((1.0f - sound_stream.source.obstructed), 0.0f, 1.0f) - al_gain_hf;
  float gain_hf = al_gain_hf + error_hf * dt;

  alSourcef(al_source, AL_GAIN, gain * sound_stream.source.gain);

  alFilteri(al_filter, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
  alFilterf(al_filter, AL_LOWPASS_GAIN , gain);      // 0.5f
  alFilterf(al_filter, AL_LOWPASS_GAINHF, gain_hf); // 0.01f
  alSourcei(al_source, AL_DIRECT_FILTER, al_filter);

  ALenum state{AL_STOPPED};
  alGetSourcei(al_source, AL_SOURCE_STATE, &state);

  ALint processed = 0;
  alGetSourcei(al_source, AL_BUFFERS_PROCESSED, &processed);
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
        alSourceQueueBuffers(al_source, 1, &buffer);
      }
      alSourcePlay(al_source);
    }

    while (processed--) {		
		  alSourceUnqueueBuffers(al_source, 1, &buffer);
                  const auto samples = sound_stream.stream->read();
                  const int size = sound_stream.stream->buffer_size;
			if (buffer != 0) {
	  			alBufferData(buffer, format, samples.data(),
					size * sizeof(ALshort),
					sound_stream.stream->sample_rate());
				alSourceQueueBuffers(al_source, 1, &buffer);
			}
    }
    if (sound_stream.source.loop && sound_stream.stream->done()) {
      sound_stream.stream->seek_start();
    }
    if (!sound_stream.source.playing && (state == AL_PLAYING)) {
      alSourceStop(al_source);
      ALint count{0};
      alGetSourcei(al_source, AL_BUFFERS_QUEUED, &count);
      alSourceUnqueueBuffers(al_source, count, &buffer);
      alDeleteBuffers(4, buffers.data());
    }
  }
}

auto Renderer::listener() const -> aud::Listener {
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

void Renderer::listener(const aud::Listener &listener) {
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

auto Renderer::render(const aud::Scene &scene, const float dt) -> void {
  listener(scene.listener);
  for (const auto &sound : scene.sounds) {
    render_sound(sound, dt);
  }
  for (const auto &sound_stream : scene.sound_streams) {
    render_sound_stream(sound_stream, dt);
  }
}

void Renderer::clear() {
  for (auto source : sources_) {
    alSourceStop(source.second);
    alDeleteSources(1, &source.second);
  }
  for (auto buffer : buffers_) {
    alDeleteBuffers(1, &buffer.second);
  }
  sources_.clear();
  buffers_.clear();
}
}
