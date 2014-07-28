/* 
 * File:   Audio.cpp
 * Author: morgan
 * 
 * Created on May 7, 2014, 9:41 PM
 */
#include "Audio.h"
#include "logging.h"
#include "Source.h"

#ifdef __ANDROID__
namespace mo {

    Audio::Audio() {
        LOGI("Starting SoundService.");
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
        startPlayer();
    }

    Audio::~Audio() {
        if (player_obj_ != NULL) {
            (*player_obj_)->Destroy(player_obj_);
            player_obj_ = NULL;
            player_ = NULL;
            player_queue_ = NULL;
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

    void Audio::startPlayer() {
        LOGI("Starting sound player.");
        SLresult result;

        // Set-up sound audio source.
        SLDataLocator_AndroidSimpleBufferQueue data_locator_in;

        data_locator_in.locatorType = SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;

        // At most one buffer in the queue.
        data_locator_in.numBuffers = 1;
        /*
                SLDataFormat_MIME data_format;
                data_format.formatType    = SL_DATAFORMAT_MIME;
                data_format.mimeType      = NULL;
                data_format.containerType = SL_CONTAINERTYPE_UNSPECIFIED;
         */
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

        try {
            result = (*engine_)->CreateAudioPlayer(engine_, &player_obj_, &data_source,
                    &data_sink, lSoundPlayerIIDCount,
                    lSoundPlayerIIDs, lSoundPlayerReqs);
            if (result != SL_RESULT_SUCCESS) throw 1;

            result = (*player_obj_)->Realize(player_obj_, SL_BOOLEAN_FALSE);
            if (result != SL_RESULT_SUCCESS) throw 1;

            result = (*player_obj_)->GetInterface(player_obj_, SL_IID_PLAY, &player_);
            if (result != SL_RESULT_SUCCESS) throw 1;

            result = (*player_obj_)->GetInterface(player_obj_, SL_IID_BUFFERQUEUE,
                    &player_queue_);
            if (result != SL_RESULT_SUCCESS) throw 1;

            // Starts the sound player. Nothing can be heard while the
            // sound queue remains empty.s
            result = (*player_)->SetPlayState(player_, SL_PLAYSTATE_PLAYING);
            if (result != SL_RESULT_SUCCESS) throw 1;
        } catch (int exception) {
            if (exception == 1) {
                LOGE("Error while creating sound player.");
            }
        }
    }

    void Audio::stop() {
        LOGI("Stopping sound engine.");

        // Stops and destroys BGM player.
        //stopBGM();

        // Destroys sound player.
        if (player_obj_ != NULL) {
            (*player_obj_)->Destroy(player_obj_);
            player_obj_ = NULL;
            player_ = NULL;
            player_queue_ = NULL;
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

    void Audio::play(const std::shared_ptr<Sound> sound) {
        SLresult result;
        SLuint32 player_state;

        (*player_obj_)->GetState(player_obj_, &player_state);
        if (player_state == SL_OBJECT_STATE_REALIZED) {


            LOGI("Before");
            int samples = std::distance(sound->begin(), sound->end());
            LOGI("Samples %d\n", samples);
            char * buffer = sound->data();
            LOGI("After");

            //int16_t * buffer = (int16_t*) pSound->mBuffer;
            //off_t    lLength = pSound->mLength;

            try {
                // Removes any sound from the queue.
                result = (*player_queue_)->Clear(player_queue_);
                if (result != SL_RESULT_SUCCESS) throw 1;

                // Plays the new sound.
                result = (*player_queue_)->Enqueue(player_queue_, buffer,
                        samples);
                if (result != SL_RESULT_SUCCESS) throw 1;
            } catch (int exception) {
                if (exception == 1) {
                    LOGE("Error playing sound.");
                }
            }
        }

    }

}
#else


namespace mo {

    Audio::Audio() {
        ALCint contextAttr[] = {ALC_FREQUENCY, 44100, 0};
        device_ = alcOpenDevice(NULL);
        context_ = alcCreateContext(device_, contextAttr);
        alcMakeContextCurrent(context_);

        auto position = glm::vec3(0.0f, 0.0f, 0.0f);
        alListener3f(AL_POSITION, position.x, position.y, position.z);
        auto velocity = glm::vec3(0.0f, 0.0f, 0.0f);
        alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);
        glm::vec3 orientation(0.0f, 0.0f, -1.0f);
        float orient[6] = {/*fwd:*/ orientation.x, orientation.y, orientation.z, /*up:*/ 0., 1., 0.};
        alListenerfv(AL_ORIENTATION, orient);
    }

    Audio::~Audio() {
    }

    void Audio::play(const Source & source) {
        if (sources_.find(source.id()) == sources_.end()) {
            ALuint al_source;
            alGenSources(1, &al_source);
            alSourcef(al_source, AL_PITCH, 1.);
            alSourcef(al_source, AL_GAIN, 1.);
            alSource3f(al_source, AL_POSITION, 0.0f, 0.0f, 0.0f);
            alSource3f(al_source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
            alSourcei(al_source, AL_LOOPING, AL_FALSE);
            sources_.insert(SourcePair(source.id(), al_source));
        }

        for (auto sound : source) {
            if (buffers_.find(sound->id()) == buffers_.end()) {
                ALuint buffer;
                alGenBuffers(1, &buffer);
                {
                    long dataSize = std::distance(sound->begin(), sound->end());
                    const ALvoid* data = sound->data();
                    alBufferData(buffer, AL_FORMAT_MONO16, data, dataSize, 44100);
                    //free( (void*)data );
                }
                alSourcei(sources_.at(source.id()), AL_BUFFER, buffer);
            }
        }
        alSourcePlay(sources_.at(source.id()));
    }

    void Audio::play(const std::shared_ptr<Sound> sound) {
        ALuint source;
        alGenSources(1, &source);
        alSourcef(source, AL_PITCH, 1.);
        alSourcef(source, AL_GAIN, 1.);
        alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
        alSource3f(source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
        alSourcei(source, AL_LOOPING, AL_FALSE);

        ALuint buffer;
        alGenBuffers(1, &buffer);
        {
            long dataSize = std::distance(sound->begin(), sound->end());
            LOGI("Samples %d\n", dataSize);
            const ALvoid* data = sound->data();
            alBufferData(buffer, AL_FORMAT_MONO16, data, dataSize, 44100);
            //free( (void*)data );
        }
        alSourcei(source, AL_BUFFER, buffer);

        alSourcePlay(source);
    }
}

#endif