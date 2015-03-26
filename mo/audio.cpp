/* 
 * File:   Audio.cpp
 * Author: morgan
 *
 * Created on May 7, 2014, 9:41 PM
 */
#include "audio.hpp"
#include "logging.hpp"
#include "source.hpp"
#include "soundsource.hpp"

#ifdef __ANDROID__
namespace mo {

Audio::Audio() : latest_player(0) {
    LOGI("Initializing audio.");
    SLresult result;
    const SLuint32 lEngineMixIIDCount = 1;
    const SLInterfaceID lEngineMixIIDs[] = {SL_IID_ENGINE};
    const SLboolean lEngineMixReqs[] = {SL_BOOLEAN_TRUE};
    const SLuint32 lOutputMixIIDCount = 0;
    const SLInterfaceID lOutputMixIIDs[] = {};
    const SLboolean lOutputMixReqs[] = {};

    try {
        // Creates OpenSL ES engine and dumps its capabilities.
        result = slCreateEngine(&engine_obj_, 0, NULL, lEngineMixIIDCount,
                                lEngineMixIIDs, lEngineMixReqs);
        if (result != SL_RESULT_SUCCESS) throw 1;

        result = (*engine_obj_)->Realize(engine_obj_, SL_BOOLEAN_FALSE);
        if (result != SL_RESULT_SUCCESS) throw 1;

        result = (*engine_obj_)->GetInterface(engine_obj_, SL_IID_ENGINE, &engine_);
        if (result != SL_RESULT_SUCCESS) throw 1;

        // Creates audio output.
        result = (*engine_)->CreateOutputMix(engine_, &output_mix_obj_,
                                             lOutputMixIIDCount, lOutputMixIIDs,
                                             lOutputMixReqs);

        result = (*output_mix_obj_)->Realize(output_mix_obj_,
                                             SL_BOOLEAN_FALSE);
    } catch (int exception) {
        if (exception == 1) {
            LOGE("Error while starting sound engine.");
            stop();
        }
    }

    // Set-up sound player.
    createBufferPlayers();
}

Audio::~Audio() {
    stop();
}

void Audio::createBufferPlayers() {
    LOGI("Starting sound players.");

    const int num_players = 4;

    SLresult result;

    // Set-up sound audio source.
    SLDataLocator_AndroidSimpleBufferQueue data_locator_in;

    data_locator_in.locatorType = SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;

    // At most one buffer in the queue.
    data_locator_in.numBuffers = 1;

    SLDataFormat_PCM data_format;
    data_format.formatType = SL_DATAFORMAT_PCM;
    data_format.numChannels = 1; // Mono sound.
    data_format.samplesPerSec = SL_SAMPLINGRATE_44_1;
    data_format.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
    data_format.containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;
    data_format.channelMask = SL_SPEAKER_FRONT_CENTER;
    data_format.endianness = SL_BYTEORDER_LITTLEENDIAN;

    SLDataSource data_source;
    data_source.pLocator = &data_locator_in;
    data_source.pFormat = &data_format;

    SLDataLocator_OutputMix data_locator_out;
    data_locator_out.locatorType = SL_DATALOCATOR_OUTPUTMIX;
    data_locator_out.outputMix = output_mix_obj_;

    SLDataSink data_sink;
    data_sink.pLocator = &data_locator_out;
    data_sink.pFormat = NULL;

    // Creates the sounds player and retrieves its interfaces.
    const SLuint32 lSoundPlayerIIDCount = 2;
    const SLInterfaceID lSoundPlayerIIDs[] = {SL_IID_PLAY, SL_IID_BUFFERQUEUE};
    const SLboolean lSoundPlayerReqs[] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

    for (int i = 0; i < num_players; i++) {
        Player player;
        try {
            result = (*engine_)->CreateAudioPlayer(engine_, &player.player_obj_, &data_source,
                                                   &data_sink, lSoundPlayerIIDCount,
                                                   lSoundPlayerIIDs, lSoundPlayerReqs);
            if (result != SL_RESULT_SUCCESS) throw 1;

            result = (*player.player_obj_)->Realize(player.player_obj_, SL_BOOLEAN_FALSE);
            if (result != SL_RESULT_SUCCESS) throw 1;

            result = (*player.player_obj_)->GetInterface(player.player_obj_, SL_IID_PLAY, &player.player_);
            if (result != SL_RESULT_SUCCESS) throw 1;

            result = (*player.player_obj_)->GetInterface(player.player_obj_, SL_IID_BUFFERQUEUE,
                                                         &player.player_queue_);
            if (result != SL_RESULT_SUCCESS) throw 1;

            // Starts the sound player. Nothing can be heard while the
            // sound queue remains empty.s
            result = (*player.player_)->SetPlayState(player.player_, SL_PLAYSTATE_PLAYING);
            if (result != SL_RESULT_SUCCESS) throw 1;
        } catch (int exception) {
            if (exception == 1) {
                LOGE("Error while creating sound player.");
            }
        }
        players_.push_back(player);
    }
}

void Audio::stop() {
    LOGI("Stopping sound engine.");

    // Stops and destroys BGM player.
    //stopBGM();

    // Destroys sound player.

    for (auto player : players_){
        if (player.player_obj_ != NULL) {
            (*player.player_obj_)->Destroy(player.player_obj_);
            player.player_obj_ = NULL;
            player.player_ = NULL;
            player.player_queue_ = NULL;
        }
    }

    if (descriptor_player_obj_ != NULL) {
        (*descriptor_player_obj_)->Destroy(descriptor_player_obj_);
        descriptor_player_obj_ = NULL;
        descriptor_player_ = NULL;
        descriptor_player_seek_ = NULL;
    }

    // Destroys audio output and engine.
    if (output_mix_obj_ != NULL) {
        (*output_mix_obj_)->Destroy(output_mix_obj_);
        output_mix_obj_ = NULL;
    }
    if (engine_obj_ != NULL) {
        (*engine_obj_)->Destroy(engine_obj_);
        engine_obj_ = NULL;
        engine_ = NULL;
    }
}

void Audio::play(const Source & source) {
    SLresult result;
    SLuint32 player_state;

    Player player = players_[latest_player];

    (*player.player_obj_)->GetState(player.player_obj_, &player_state);
    if (player_state == SL_OBJECT_STATE_REALIZED) {

        for (auto sound : source) {
            int samples = std::distance(sound->begin(), sound->end());
            LOGI("samples %d", samples);
            const short * buffer = sound->data();

            try {
                // Removes any sound from the queue.
                result = (*player.player_queue_)->Clear(player.player_queue_);
                if (result != SL_RESULT_SUCCESS) throw 1;

                // Plays the new sound.
                result = (*player.player_queue_)->Enqueue(player.player_queue_, buffer,
                                                          samples * sizeof (short));
                if (result != SL_RESULT_SUCCESS) throw 1;
            } catch (int exception) {
                if (exception == 1) {
                    LOGE("Error playing sound.");
                }
            }
        }
    }
    latest_player ++;
    if (latest_player > 3) {
        latest_player = 0;
    };

}

void Audio::playStream(const std::string file_name, const Assets & assets) {
    SLresult result;
    LOGI("Opening music file %s", file_name.c_str());

    Descriptor descriptor = assets.descript(file_name);
    if (descriptor.descriptor < 0) {
        LOGI("Could not open music file");
        throw 1;
    }

    SLDataLocator_AndroidFD data_locator_in;
    data_locator_in.locatorType = SL_DATALOCATOR_ANDROIDFD;
    data_locator_in.fd = descriptor.descriptor;
    data_locator_in.offset = descriptor.start;
    data_locator_in.length = descriptor.length;

    SLDataFormat_MIME data_format;
    data_format.formatType = SL_DATAFORMAT_MIME;
    data_format.mimeType = NULL;
    data_format.containerType = SL_CONTAINERTYPE_UNSPECIFIED;

    SLDataSource data_source;
    data_source.pLocator = &data_locator_in;
    data_source.pFormat = &data_format;

    SLDataLocator_OutputMix data_locator_out;
    data_locator_out.locatorType = SL_DATALOCATOR_OUTPUTMIX;
    data_locator_out.outputMix = output_mix_obj_;

    SLDataSink data_sink;
    data_sink.pLocator = &data_locator_out;
    data_sink.pFormat = NULL;

    // Creates BGM player and retrieves its interfaces.
    const SLuint32 lBGMPlayerIIDCount = 2;
    const SLInterfaceID lBGMPlayerIIDs[] = {SL_IID_PLAY, SL_IID_SEEK};
    const SLboolean lBGMPlayerReqs[] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

    result = (*engine_)->CreateAudioPlayer(engine_,
                                           &descriptor_player_obj_, &data_source, &data_sink,
                                           lBGMPlayerIIDCount, lBGMPlayerIIDs, lBGMPlayerReqs);
    if (result != SL_RESULT_SUCCESS) throw 1;
    result = (*descriptor_player_obj_)->Realize(descriptor_player_obj_,
                                                SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) throw 1;

    result = (*descriptor_player_obj_)->GetInterface(descriptor_player_obj_,
                                                     SL_IID_PLAY, &descriptor_player_);
    if (result != SL_RESULT_SUCCESS) throw 1;
    result = (*descriptor_player_obj_)->GetInterface(descriptor_player_obj_,
                                                     SL_IID_SEEK, &descriptor_player_seek_);
    if (result != SL_RESULT_SUCCESS) throw 1;

    // Enables looping and starts playing.
    result = (*descriptor_player_seek_)->SetLoop(descriptor_player_seek_,
                                                 SL_BOOLEAN_TRUE, 0, SL_TIME_UNKNOWN);
    if (result != SL_RESULT_SUCCESS) throw 1;
    result = (*descriptor_player_)->SetPlayState(descriptor_player_,
                                                 SL_PLAYSTATE_PLAYING);
    if (result != SL_RESULT_SUCCESS) throw 1;
}

}
#else

#include <iostream>
#include <cstring>
#include <future>
#include <thread>
#include <glm/gtx/io.hpp>

namespace mo {

Audio::Audio() {
    ALCint contextAttr[] = {ALC_FREQUENCY, 44100, 0};
    device_ = alcOpenDevice(NULL);
    context_ = alcCreateContext(device_, contextAttr);
    alcMakeContextCurrent(context_);

    listener_position(glm::vec3(0.0f));
    listener_velocity(glm::vec3(0.0f));
    listener_orientation(glm::vec3(0.0f, 0.0f, -1.0f));
}

Audio::~Audio() {
    for (auto source : sources_){
        alDeleteSources(1, & source.second);
    }
    for (auto buffer : buffers_){
        alDeleteBuffers(1, & buffer.second);
    }
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(context_);
    alcCloseDevice(device_);
}


glm::vec3 Audio::listener_position() {
    glm::vec3 position;
    alGetListener3f(AL_POSITION, & position.x, & position.y, & position.z);
    return position;
}

void Audio::listener_position(const glm::vec3 position) {
    alListener3f(AL_POSITION, position.x, position.y, position.z);
}

glm::vec3 Audio::listener_orientation() {
    glm::vec3 orientation;
    alGetListener3f(AL_ORIENTATION, & orientation.x, & orientation.y, & orientation.z);
    return orientation;
}

void Audio::listener_orientation(const glm::vec3 orientation, const glm::vec3 up)
{
    float orient[6] = {/*fwd:*/ orientation.x,orientation.y, orientation.z,
                       /*up:*/ up.x, up.y, up.z};
    alListenerfv(AL_ORIENTATION, orient);
}

glm::vec3 Audio::listener_velocity() {
    glm::vec3 velocity;
    alGetListener3f(AL_VELOCITY, & velocity.x, & velocity.y, & velocity.z);
    return velocity;
}

void Audio::listener_velocity(const glm::vec3 velocity) {
    alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);
}


void Audio::init(const SoundSource & source) {
    if (sources_.find(source.id()) == sources_.end()) {
        ALuint al_source;
        alGenSources(1, &al_source);
        sources_.insert(SourcePair(source.id(), al_source));
        //update(source);
    }

    auto sound = source.sound;
    if (buffers_.find(sound->id()) == buffers_.end()) {
        ALuint buffer;
        alGenBuffers(1, &buffer);
        {
            long data_size = std::distance(sound->begin(), sound->end());
            const ALvoid * data = sound->data();
            alBufferData(buffer, AL_FORMAT_MONO16, data, data_size * sizeof (short), 44100);
        }
        alSourcei(sources_.at(source.id()), AL_BUFFER, buffer);
        buffers_.insert(BufferPair(sound->id(), buffer));
    }
    alSource3f(sources_.at(source.id()), AL_POSITION, source.position.x, source.position.y, source.position.z);
    //alSourcePlay(sources_.at(source.id()));
}



void Audio::init(const StreamSource & stream_source) {
    stream_threads.push_back(std::thread([&](StreamSource stream_source) {
                                 ALuint buffers[2];
                                 ALuint source;
                                 if (sources_.find(stream_source.id()) == sources_.end()) {
                                     alGenSources(1, &source);
                                     sources_.insert(SourcePair(stream_source.id(), source));
                                 } else {
                                     source = sources_.at(stream_source.id());
                                 }
                                 alGenBuffers(2, buffers);

                                 int size = stream_source.stream->buffer_size;
                                 alBufferData(buffers[0], AL_FORMAT_MONO16, stream_source.stream->read().data(), size*sizeof(ALshort), stream_source.stream->sample_rate());
                                 alBufferData(buffers[1], AL_FORMAT_MONO16, stream_source.stream->read().data(), size*sizeof(ALshort), stream_source.stream->sample_rate());

                                 alSourceQueueBuffers(source, 2, buffers);
                                 alSource3f(source, AL_POSITION, stream_source.position.x, stream_source.position.y, stream_source.position.z);
                                 if (stream_source.playing) {
                                     alSourcePlay(source);
                                 }
                                 while(true) {
                                     ALint processed = 0;
                                     alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
                                     while(processed--){
                                         ALuint buffer = 0;
                                         alSourceUnqueueBuffers(source, 1, &buffer);
                                         auto samples = stream_source.stream->read();
                                         alBufferData(buffer, AL_FORMAT_MONO16, samples.data(), size*sizeof(ALshort), stream_source.stream->sample_rate());
                                         alSourceQueueBuffers(source, 1, &buffer);
                                     }
                                     if (stream_source.loop && stream_source.stream->done()){
                                         stream_source.stream->seek_start();
                                         stream_source.playing = true;
                                     }
                                     else{
                                         stream_source.playing = false;
                                     }
                                 }
                                 alDeleteSources(1, & source);
                                 alDeleteBuffers(2, buffers);
                             }, stream_source));
}

void Audio::update(Source & source)
{
    if (sources_.find(source.id()) != sources_.end()) {
        ALuint al_source = sources_.at(source.id());
        alSourcef(al_source, AL_PITCH, source.pitch);
        alSourcef(al_source, AL_GAIN, source.gain);
        alSource3f(al_source, AL_POSITION, source.position.x,
                   source.position.y, source.position.z);
        alSource3f(al_source, AL_VELOCITY, source.velocity.x, source.velocity.y, source.velocity.z);

        ALenum state;
        alGetSourcei(al_source, AL_SOURCE_STATE, &state);

        if (source.playing && (state != AL_PLAYING)){
            alSourcePlay(al_source);
        }

        if(!source.playing && (state == AL_PLAYING)) {
            alSourceStop(al_source);
        }

        if (state == AL_STOPPED){
            alSourceRewind(al_source);
            source.playing = false;
        }

    }
}


}

#endif
