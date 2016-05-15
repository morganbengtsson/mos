#include <chrono>
#include <glm/gtx/io.hpp>
#include <iostream>
#include <thread>

#include <mos/audio/player.hpp>
#include <mos/audio/soundsource.hpp>
#include <mos/audio/source.hpp>

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

#endif //MOS_EFX
namespace mos {

Player::Player()
    : reverb_properties(EFX_REVERB_PRESET_LIVINGROOM), reverb_effect(0),
      reverb_slot(0), lowpass_filter1(0), lowpass_filter2(0) {
  ALCint contextAttr[] = {ALC_FREQUENCY, 44100, 0};
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

Player::~Player() {
  for (auto &thread : stream_threads) {
    thread.second.running = false;
    thread.second.thread.join();
  }
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

void Player::load(const SoundSource &sound_source) {
  if (sources_.find(sound_source.source.id()) == sources_.end()) {
    ALuint al_source;
    alGenSources(1, &al_source);
    sources_.insert(SourcePair(sound_source.source.id(), al_source));


#ifdef MOS_EFX
    alSource3i(al_source, AL_AUXILIARY_SEND_FILTER, reverb_slot, 0,
               AL_FILTER_NULL);

    ALuint al_filter;
    alGenFilters(1, &al_filter);
    filters_.insert(SourcePair(sound_source.source.id(), al_filter));
    alFilteri(al_filter, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
    alSourcei(al_source, AL_DIRECT_FILTER, al_filter);
#endif
  }

  auto sound = sound_source.sound;
  if (buffers_.find(sound->id()) == buffers_.end()) {
    ALuint buffer;
    alGenBuffers(1, &buffer);
    {
      long data_size = std::distance(sound->begin(), sound->end());
      const ALvoid *data = sound->data();
      alBufferData(buffer, AL_FORMAT_MONO16, data, data_size * sizeof(short),
                   sound->sample_rate());
    }
    buffers_.insert(BufferPair(sound->id(), buffer));
  }
  alSourcei(sources_.at(sound_source.source.id()), AL_BUFFER,
            buffers_.at(sound->id()));
  alSource3f(sources_.at(sound_source.source.id()), AL_POSITION,
             sound_source.source.position.x, sound_source.source.position.y,
             sound_source.source.position.z);
  // alSourcePlay(sources_.at(source.id()));
}

bool Player::loaded(const SoundSource &sound_source) {
  if (sources_.find(sound_source.source.id()) == sources_.end()) {
    return false;
  } else {
    return true;
  }
}

void Player::load(const StreamSource &stream_source) {}

void Player::unload(const SoundSource &sound_source) {}

void Player::unload(const StreamSource &stream_source) {}

void Player::update(StreamSource &sound_source, const float dt) {
  if (sources_.find(sound_source.source.id()) == sources_.end()) {
    ALuint al_source;
    alGenSources(1, &al_source);
    sources_.insert(SourcePair(sound_source.source.id(), al_source));

#ifdef MOS_EFX
    alSource3i(al_source, AL_AUXILIARY_SEND_FILTER, reverb_slot, 0,
               AL_FILTER_NULL);
    ALuint al_filter;
    alGenFilters(1, &al_filter);
    filters_.insert(SourcePair(sound_source.source.id(), al_filter));
    alFilteri(al_filter, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
    alSourcei(al_source, AL_DIRECT_FILTER, al_filter);
#endif
  };

  ALuint al_source = sources_.at(sound_source.source.id());
  alSourcei(al_source, AL_LOOPING, sound_source.source.loop);
  alSourcef(al_source, AL_PITCH, sound_source.source.pitch);
  alSourcef(al_source, AL_GAIN, sound_source.source.gain);
  alSource3f(al_source, AL_POSITION, sound_source.source.position.x,
             sound_source.source.position.y, sound_source.source.position.z);
  alSource3f(al_source, AL_VELOCITY, sound_source.source.velocity.x,
             sound_source.source.velocity.y, sound_source.source.velocity.z);

#ifdef MOS_EFX
  auto al_filter = filters_[sound_source.source.id()];
  float ob = sound_source.source.obstructed >= 1.0f ? -1.0f : 1.0f;
  ALfloat al_gain;
  alGetFilterf(al_filter, AL_LOWPASS_GAIN, &al_gain);
  float gain = glm::clamp(al_gain + dt * ob, 0.5f, 1.0f);

  ALfloat al_gain_hf;
  alGetFilterf(al_filter, AL_LOWPASS_GAINHF, &al_gain_hf);
  float gain_hf = glm::clamp(al_gain_hf + dt * ob, 0.01f, 1.0f);

  alFilteri(al_filter, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
  alFilterf(al_filter, AL_LOWPASS_GAIN, gain);      // 0.5f
  alFilterf(al_filter, AL_LOWPASS_GAINHF, gain_hf); // 0.01f
  alSourcei(al_source, AL_DIRECT_FILTER, al_filter);
#endif

  sound_source.source.obstructed = 0.0f;

  ALenum state;
  alGetSourcei(al_source, AL_SOURCE_STATE, &state);

  if (sound_source.source.playing && (state != AL_PLAYING)) {
    if (stream_threads.count(sound_source.stream->id())) {
      stream_threads[sound_source.stream->id()].running = false;
      stream_threads[sound_source.stream->id()].thread.join();
      stream_threads.erase(sound_source.stream->id());
    }
    stream_threads.insert(std::pair<unsigned int, StreamThread>(
        sound_source.stream->id(),
        StreamThread{
            std::thread(
                [&](ALuint al_source, std::shared_ptr<mos::Stream> stream_ptr,
                    const bool loop) {

                  ALuint buffers[4]; // TODO std array
                  alGenBuffers(4, buffers);
                  int size = stream_ptr->buffer_size;
                  for (int i = 0; i < 4; i++) {
                    alBufferData(buffers[i], AL_FORMAT_MONO16, stream_ptr->read().data(),
                        size * sizeof(ALshort), stream_ptr->sample_rate());
                    alSourceQueueBuffers(al_source, 1, &buffers[i]);
                  }

                  alSourcePlay(al_source);
                  alSourcei(al_source, AL_STREAMING, AL_TRUE);

                  while (stream_threads[sound_source.stream->id()].running) {
                    ALint processed = 0;
                    alGetSourcei(al_source, AL_BUFFERS_PROCESSED, &processed);
                    while (
                        processed-- &&
                        (stream_threads[sound_source.stream->id()].running)) {
                      ALuint buffer = 0;
                      alSourceUnqueueBuffers(al_source, 1, &buffer);
                      auto samples = stream_ptr->read();
                      alBufferData(buffer, AL_FORMAT_MONO16, samples.data(),
                                   size * sizeof(ALshort),
                                   stream_ptr->sample_rate());
                      alSourceQueueBuffers(al_source, 1, &buffer);
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    if (loop && stream_ptr->done()) {
                      stream_ptr->seek_start();
                    }
                  }
                  stream_ptr->seek_start();
                  alDeleteBuffers(2, buffers);
                },
                al_source, sound_source.stream, sound_source.source.loop),
            true}));
  }

  ALint type;
  alGetSourcei(al_source, AL_SOURCE_TYPE, &type);
  if (!sound_source.source.playing && (state == AL_PLAYING)) {
    alSourceStop(al_source);
  }

  /*
  if (state == AL_STOPPED) {
      alSourceRewind(al_source);
      source.playing = false;
  }*/
}

Listener Player::listener() const {
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

void Player::listener(const Listener &listener) {
  alListener3f(AL_POSITION, listener.position.x, listener.position.y,
               listener.position.z);

  alListener3f(AL_VELOCITY, listener.velocity.x, listener.velocity.y,
               listener.velocity.z);

  float orientation[6] = {listener.direction.x, listener.direction.y,
                          listener.direction.z, listener.up.x,
                          listener.up.y,        listener.up.z};
  alListenerfv(AL_ORIENTATION, orientation);
}

void Player::update(const SoundSource &sound_source, const float dt) {
  if (sources_.find(sound_source.source.id()) != sources_.end()) {
    ALuint al_source = sources_.at(sound_source.source.id());
    alSourcei(al_source, AL_LOOPING, sound_source.source.loop);
    alSourcef(al_source, AL_PITCH, sound_source.source.pitch);
    alSourcef(al_source, AL_GAIN, sound_source.source.gain);
    alSource3f(al_source, AL_POSITION, sound_source.source.position.x,
               sound_source.source.position.y, sound_source.source.position.z);
    alSource3f(al_source, AL_VELOCITY, sound_source.source.velocity.x,
               sound_source.source.velocity.y, sound_source.source.velocity.z);

#ifdef MOS_EFX
    auto al_filter = filters_[sound_source.source.id()];
    float ob = sound_source.source.obstructed >= 1.0f ? -1.0f : 1.0f;
    ALfloat al_gain;
    alGetFilterf(al_filter, AL_LOWPASS_GAIN, &al_gain);
    float gain = glm::clamp(al_gain + dt * ob, 0.5f, 1.0f);

    ALfloat al_gain_hf;
    alGetFilterf(al_filter, AL_LOWPASS_GAINHF, &al_gain_hf);
    float gain_hf = glm::clamp(al_gain_hf + dt * ob, 0.01f, 1.0f);

    alFilteri(al_filter, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
    alFilterf(al_filter, AL_LOWPASS_GAIN, gain);      // 0.5f
    alFilterf(al_filter, AL_LOWPASS_GAINHF, gain_hf); // 0.01f
    alSourcei(al_source, AL_DIRECT_FILTER, al_filter);
#endif

    // sound_source.source.obstructed = 0.0f;

    ALenum state;
    alGetSourcei(al_source, AL_SOURCE_STATE, &state);

    if (sound_source.source.playing && (state != AL_PLAYING)) {
      alSourcePlay(al_source);
    }

    ALint type;
    alGetSourcei(al_source, AL_SOURCE_TYPE, &type);

    if (!sound_source.source.playing && (state == AL_PLAYING)) {
      alSourceStop(al_source);
    }

    if (state == AL_STOPPED) {
      alSourceRewind(al_source);
    }
  }
}
}
