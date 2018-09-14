#include <chrono>
#include <glm/gtx/io.hpp>
#include <iostream>
#include <thread>

#include <mos/aud/buffer_source.hpp>
#include <mos/aud/renderer.hpp>

#ifdef MOS_EFX

/* Effect object functions */
static LPALGENEFFECTS alGenEffects;
static LPALDELETEEFFECTS alDeleteEffects;
static LPALISEFFECT alIsEffect;
static LPALEFFECTI alEffecti;
static LPALEFFECTIV alEffectiv;
static LPALEFFECTF alEffectf;
static LPALEFFECTFV alEffectfv;
static LPALGETEFFECTI alGetEffecti;
static LPALGETEFFECTIV alGetEffectiv;
static LPALGETEFFECTF alGetEffectf;
static LPALGETEFFECTFV alGetEffectfv;

// Filter object functions
static LPALGENFILTERS alGenFilters;
static LPALDELETEFILTERS alDeleteFilters;
static LPALFILTERI alFilteri;
static LPALFILTERF alFilterf;
static LPALGETFILTERI alGetFilteri;
static LPALGETFILTERF alGetFilterf;

/* Auxiliary Effect Slot object functions */
static LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots;
static LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots;
static LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot;
static LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti;
static LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv;
static LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf;
static LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv;
static LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti;
static LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv;
static LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf;
static LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv;

void init_efx() {

  alGenEffects = (LPALGENEFFECTS)alGetProcAddress("alGenEffects");
  alDeleteEffects = (LPALDELETEEFFECTS)alGetProcAddress("alDeleteEffects");
  alIsEffect = (LPALISEFFECT)alGetProcAddress("alIsEffect");
  alEffecti = (LPALEFFECTI)alGetProcAddress("alEffecti");
  alEffectiv = (LPALEFFECTIV)alGetProcAddress("alEffectiv");
  alEffectf = (LPALEFFECTF)alGetProcAddress("alEffectf");
  alEffectfv = (LPALEFFECTFV)alGetProcAddress("alEffectfv");
  alGetEffecti = (LPALGETEFFECTI)alGetProcAddress("alGetEffecti");
  alGetEffectiv = (LPALGETEFFECTIV)alGetProcAddress("alGetEffectiv");
  alGetEffectf = (LPALGETEFFECTF)alGetProcAddress("alGetEffectf");
  alGetEffectfv = (LPALGETEFFECTFV)alGetProcAddress("alGetEffectfv");

  alGenFilters = (LPALGENFILTERS)alGetProcAddress("alGenFilters");
  alDeleteFilters = (LPALDELETEFILTERS)alGetProcAddress("alDeleteFilters");
  alFilteri = (LPALFILTERI)alGetProcAddress("alFilteri");
  alFilterf = (LPALFILTERF)alGetProcAddress("alFilterf");
  alGetFilteri = (LPALGETFILTERI)alGetProcAddress("alGetFilteri");
  alGetFilterf = (LPALGETFILTERF)alGetProcAddress("alGetFilterf");

  /* Auxiliary Effect Slot object functions */
  alGenAuxiliaryEffectSlots = (LPALGENAUXILIARYEFFECTSLOTS)alGetProcAddress(
      "alGenAuxiliaryEffectSlots");
  alDeleteAuxiliaryEffectSlots =
      (LPALDELETEAUXILIARYEFFECTSLOTS)alGetProcAddress(
          "alDeleteAuxiliaryEffectSlots");
  alIsAuxiliaryEffectSlot =
      (LPALISAUXILIARYEFFECTSLOT)alGetProcAddress("alIsAuxiliaryEffectSlot");
  alAuxiliaryEffectSloti =
      (LPALAUXILIARYEFFECTSLOTI)alGetProcAddress("alAuxiliaryEffectSloti");
  alAuxiliaryEffectSlotiv =
      (LPALAUXILIARYEFFECTSLOTIV)alGetProcAddress("alAuxiliaryEffectSlotiv");
  alAuxiliaryEffectSlotf =
      (LPALAUXILIARYEFFECTSLOTF)alGetProcAddress("alAuxiliaryEffectSlotf");
  alAuxiliaryEffectSlotfv =
      (LPALAUXILIARYEFFECTSLOTFV)alGetProcAddress("alAuxiliaryEffectSlotfv");
  alGetAuxiliaryEffectSloti = (LPALGETAUXILIARYEFFECTSLOTI)alGetProcAddress(
      "alGetAuxiliaryEffectSloti");
  alGetAuxiliaryEffectSlotiv = (LPALGETAUXILIARYEFFECTSLOTIV)alGetProcAddress(
      "alGetAuxiliaryEffectSlotiv");
  alGetAuxiliaryEffectSlotf = (LPALGETAUXILIARYEFFECTSLOTF)alGetProcAddress(
      "alGetAuxiliaryEffectSlotf");
  alGetAuxiliaryEffectSlotfv = (LPALGETAUXILIARYEFFECTSLOTFV)alGetProcAddress(
      "alGetAuxiliaryEffectSlotfv");
}

#endif // MOS_EFX
namespace mos {
namespace aud {

Renderer::Renderer()
    : reverb_properties(EFX_REVERB_PRESET_LIVINGROOM), reverb_effect(0),
      reverb_slot(0), lowpass_filter1(0), lowpass_filter2(0) {
  ALCint contextAttr[] = {ALC_FREQUENCY, 44100, ALC_HRTF_SOFT, ALC_TRUE, 0};
  device_ = alcOpenDevice(NULL);
  context_ = alcCreateContext(device_, contextAttr);
  alcMakeContextCurrent(context_);

#ifdef MOS_EFX
  if (!alcIsExtensionPresent(alcGetContextsDevice(alcGetCurrentContext()),
                             "ALC_EXT_EFX")) {
    throw std::runtime_error("OpenAL EFX not supported.");
  }

  init_efx();

  ALuint reverb_effect = 0;
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

  if (!reverb_effect) {
    throw std::runtime_error("Could not create reverb effect.");
  }

  alGenAuxiliaryEffectSlots(1, &reverb_slot);
  if (!reverb_slot) {
    throw std::runtime_error("Could not create reverb effect slot.");
  }

  alAuxiliaryEffectSloti(reverb_slot, AL_EFFECTSLOT_EFFECT, reverb_effect);

  alGenFilters(1, &lowpass_filter1);
  if (!lowpass_filter1) {
    std::runtime_error("Could not create lowpass filter.");
  }
  alFilteri(lowpass_filter1, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
  alFilterf(lowpass_filter1, AL_LOWPASS_GAIN, 0.6f);    // 0.5f
  alFilterf(lowpass_filter1, AL_LOWPASS_GAINHF, 0.05f); // 0.01f

  alGenFilters(1, &lowpass_filter2);
  if (!lowpass_filter2) {
    std::runtime_error("Could not create lowpass filter.");
  }
  alFilteri(lowpass_filter2, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
  alFilterf(lowpass_filter2, AL_LOWPASS_GAIN, 0.3f);    // 0.5f
  alFilterf(lowpass_filter2, AL_LOWPASS_GAINHF, 0.01f); // 0.01f
#endif

  listener(Listener());
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

void Renderer::buffer_source(const Buffer_source &buffer_source) {

  //TODO: HACK, replace this!
  float dt = 1.0f / 60.0f;

  if (sources_.find(buffer_source.source.id()) == sources_.end()) {
    ALuint al_source;
    alGenSources(1, &al_source);
    sources_.insert(SourcePair(buffer_source.source.id(), al_source));

#ifdef MOS_EFX
    alSource3i(al_source, AL_AUXILIARY_SEND_FILTER, reverb_slot, 0,
               AL_FILTER_NULL);
    ALuint al_filter;
    alGenFilters(1, &al_filter);
    filters_.insert(SourcePair(buffer_source.source.id(), al_filter));
    alFilteri(al_filter, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
    alSourcei(al_source, AL_DIRECT_FILTER, al_filter);
#endif
  }

  auto buffer = buffer_source.buffer;
  auto format = buffer->channels() == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
  if (buffers_.find(buffer->id()) == buffers_.end()) {
    ALuint al_buffer;
    alGenBuffers(1, &al_buffer);
    {
      long data_size = std::distance(buffer->begin(), buffer->end());
      const ALvoid *data = buffer->data();
      alBufferData(al_buffer, format, data, data_size * sizeof(short),
                   buffer->sample_rate());
    }
    buffers_.insert(BufferPair(buffer->id(), al_buffer));
	
	alSourcei(sources_.at(buffer_source.source.id()), AL_BUFFER,
		buffers_.at(buffer->id()));
  }  

  ALuint al_source = sources_.at(buffer_source.source.id());
  alSourcei(al_source, AL_LOOPING, buffer_source.source.loop);
  alSourcef(al_source, AL_PITCH, buffer_source.source.pitch);
  //alSourcef(al_source, AL_GAIN, buffer_source.source.gain);
  alSource3f(al_source, AL_POSITION, buffer_source.source.position.x,
             buffer_source.source.position.y, buffer_source.source.position.z);
  alSource3f(al_source, AL_VELOCITY, buffer_source.source.velocity.x,
             buffer_source.source.velocity.y, buffer_source.source.velocity.z);

#ifdef MOS_EFX
  auto al_filter = filters_[buffer_source.source.id()];
  ALfloat al_gain;
  alGetFilterf(al_filter, AL_LOWPASS_GAIN, &al_gain);
  auto error = glm::clamp((1.0f - buffer_source.source.obstructed), 0.0f, 1.0f) - al_gain;
  float gain = al_gain + error * dt;

  ALfloat al_gain_hf;
  alGetFilterf(al_filter, AL_LOWPASS_GAINHF, &al_gain_hf);
  auto error_hf = glm::clamp((1.0f - buffer_source.source.obstructed), 0.0f, 1.0f) - al_gain_hf;
  float gain_hf = al_gain_hf + error_hf * dt;

  alSourcef(al_source, AL_GAIN, gain * buffer_source.source.gain);

  alFilteri(al_filter, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
  alFilterf(al_filter, AL_LOWPASS_GAIN, gain);      // 0.5f
  alFilterf(al_filter, AL_LOWPASS_GAINHF, gain_hf); // 0.01f
  alSourcei(al_source, AL_DIRECT_FILTER, al_filter);
#endif

  ALenum state;
  alGetSourcei(al_source, AL_SOURCE_STATE, &state);

  if (buffer_source.source.playing && (state != AL_PLAYING)) {
    alSourcePlay(al_source);
  }

  ALint type;
  alGetSourcei(al_source, AL_SOURCE_TYPE, &type);

  if (!buffer_source.source.playing && (state == AL_PLAYING)) {
    alSourceStop(al_source);
  }

  if (state == AL_STOPPED) {
    alSourceRewind(al_source);
  }
}

void Renderer::stream_source(const Stream_source &stream_source) {

  float dt = 1.0f / 60.0f; // TODO: REMOVE HACK

  if (sources_.find(stream_source.source.id()) == sources_.end()) {
    ALuint al_source;
    alGenSources(1, &al_source);
    sources_.insert(SourcePair(stream_source.source.id(), al_source));

#ifdef MOS_EFX
    alSource3i(al_source, AL_AUXILIARY_SEND_FILTER, reverb_slot, 0,
               AL_FILTER_NULL);
    ALuint al_filter;
    alGenFilters(1, &al_filter);
    filters_.insert(SourcePair(stream_source.source.id(), al_filter));
    alFilteri(al_filter, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
    alSourcei(al_source, AL_DIRECT_FILTER, al_filter);
#endif
  };

  ALuint al_source = sources_.at(stream_source.source.id());
  alSourcei(al_source, AL_LOOPING, stream_source.source.loop);
  alSourcef(al_source, AL_PITCH, stream_source.source.pitch);
  //alSourcef(al_source, AL_GAIN, stream_source.source.gain);
  alSource3f(al_source, AL_POSITION, stream_source.source.position.x,
             stream_source.source.position.y, stream_source.source.position.z);
  alSource3f(al_source, AL_VELOCITY, stream_source.source.velocity.x,
             stream_source.source.velocity.y, stream_source.source.velocity.z);

#ifdef MOS_EFX
  auto al_filter = filters_[stream_source.source.id()];
  ALfloat al_gain;
  alGetFilterf(al_filter, AL_LOWPASS_GAIN, &al_gain);
  auto error = glm::clamp((1.0f - stream_source.source.obstructed), 0.0f, 1.0f) - al_gain;
  float gain = al_gain + error * dt;

  ALfloat al_gain_hf;
  alGetFilterf(al_filter, AL_LOWPASS_GAINHF, &al_gain_hf);
  auto error_hf = glm::clamp((1.0f - stream_source.source.obstructed), 0.0f, 1.0f) - al_gain_hf;
  float gain_hf = al_gain_hf + error_hf * dt;

  alSourcef(al_source, AL_GAIN, gain * stream_source.source.gain);

  alFilteri(al_filter, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
  alFilterf(al_filter, AL_LOWPASS_GAIN , gain);      // 0.5f
  alFilterf(al_filter, AL_LOWPASS_GAINHF, gain_hf); // 0.01f
  alSourcei(al_source, AL_DIRECT_FILTER, al_filter);
#endif

  ALenum state;
  alGetSourcei(al_source, AL_SOURCE_STATE, &state);

  ALint processed = 0;
  alGetSourcei(al_source, AL_BUFFERS_PROCESSED, &processed);
  ALuint buffer = 0;

  if(stream_source.stream) {
    auto format = stream_source.stream->channels() == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

    ALuint buffers[4]; // TODO std array
    if (stream_source.source.playing && (state != AL_PLAYING)) {
      alGenBuffers(4, buffers);
      int size = stream_source.stream->buffer_size;
      for (int i = 0; i < 4; i++) {
        alBufferData(
            buffers[i], format, stream_source.stream->read().data(),
            size * sizeof(ALshort), stream_source.stream->sample_rate());
        alSourceQueueBuffers(al_source, 1, &buffers[i]);
      }
      alSourcePlay(al_source);
      alSourcei(al_source, AL_STREAMING, AL_TRUE);
    }

    while (processed--) {
      alSourceUnqueueBuffers(al_source, 1, &buffer);
      auto samples = stream_source.stream->read();
      int size = stream_source.stream->buffer_size;

	  if (buffer != 0) {
		  alBufferData(buffer, format, samples.data(),
			  size * sizeof(ALshort),
			  stream_source.stream->sample_rate());
		  alSourceQueueBuffers(al_source, 1, &buffer);
	  }
    }
    if (stream_source.source.loop && stream_source.stream->done()) {
      stream_source.stream->seek_start();
    }
    if (!stream_source.source.playing && (state == AL_PLAYING)) {
      alSourceStop(al_source);
      ALint count;
      alGetSourcei(al_source, AL_BUFFERS_QUEUED, &count);
      alSourceUnqueueBuffers(al_source, count, &buffer);
      alDeleteBuffers(4, buffers);
    }
  }
}

Listener Renderer::listener() const {
  Listener listener;
  alGetListener3f(AL_POSITION, &listener.position.x, &listener.position.y,
                  &listener.position.z);

  float orientation[6];
  alGetListenerf(AL_ORIENTATION, orientation);
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

void Renderer::listener(const Listener &listener) {
  alListener3f(AL_POSITION, listener.position.x, listener.position.y,
               listener.position.z);

  alListener3f(AL_VELOCITY, listener.velocity.x, listener.velocity.y,
               listener.velocity.z);

  float orientation[6] = {listener.direction.x, listener.direction.y,
                          listener.direction.z, listener.up.x,
                          listener.up.y, listener.up.z};
  alListenerfv(AL_ORIENTATION, orientation);
  alListenerf(AL_GAIN, listener.gain);
}

void Renderer::render(const Scene &scene) {
  listener(scene.listener);
  for (const auto &bs : scene.buffer_sources) {
    buffer_source(bs);
  }
  for (auto &ss : scene.stream_sources) {
    stream_source(ss);
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
}
