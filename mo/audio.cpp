/* 
 * File:   Audio.cpp
 * Author: morgan
 * 
 * Created on May 7, 2014, 9:41 PM
 */
#include "audio.hpp"
#include "logging.hpp"
#include "source.hpp"

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

#include <stb_vorbis.h>
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
        thread_->join();
        delete thread_;
    }
    void Audio::stop() {

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

    void Audio::listener_velocity(const glm::vec3 velocity)
    {
        alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);
    }

    
    void Audio::play(const Source & source) {
        if (sources_.find(source.id()) == sources_.end()) {
            ALuint al_source;
            alGenSources(1, &al_source);
            alSourcef(al_source, AL_PITCH, 1.0f);
            alSourcef(al_source, AL_GAIN, 1.0f);
            alSource3f(al_source, AL_POSITION, source.position.x, source.position.y, source.position.z);
            alSource3f(al_source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
            alSourcei(al_source, AL_LOOPING, source.loop);
            sources_.insert(SourcePair(source.id(), al_source));
        }

        for (auto sound : source) {
            if (buffers_.find(sound->id()) == buffers_.end()) {
                ALuint buffer;
                alGenBuffers(1, &buffer);
                {
                    long dataSize = std::distance(sound->begin(), sound->end());
                    const ALvoid* data = sound->data();
                    alBufferData(buffer, AL_FORMAT_MONO16, data, dataSize * sizeof (short), 44100);
                    //free( (void*)data );
                }
                alSourcei(sources_.at(source.id()), AL_BUFFER, buffer);
            }
        }
        alSourcePlay(sources_.at(source.id()));
        alSource3f(sources_.at(source.id()), AL_POSITION, source.position.x, source.position.y, source.position.z);
    }

   
    
    struct AudioStream {
	ALuint ID;
 
	stb_vorbis* stream;
	stb_vorbis_info info;
 
	ALuint buffers[2];
	ALuint source;
	ALenum format;
 
	size_t bufferSize;
 
	size_t totalSamplesLeft;
 
	bool shouldLoop;
};
 
void AudioStreamInit(AudioStream* self){
	memset(self, 0, sizeof(AudioStream));
	alGenSources(1, & self->source);
	alGenBuffers(2, self->buffers);
	self->bufferSize=4096*8;
	self->shouldLoop=false;//We loop by default
}
 
void AudioStreamDeinit(AudioStream* self){
	alDeleteSources(1, & self->source);
	alDeleteBuffers(2, self->buffers);
	stb_vorbis_close(self->stream);
	memset(self, 0, sizeof(AudioStream));
}
 
bool AudioStreamStream(AudioStream* self, ALuint buffer){
	//Uncomment this to avoid VLAs
	#define BUFFER_SIZE 4096*32
	#ifndef BUFFER_SIZE//VLAs ftw
	#define BUFFER_SIZE (self->bufferSize)
	#endif
	ALshort pcm[BUFFER_SIZE];
	int  size = 0;
	int  result = 0;
 
	while(size < BUFFER_SIZE){
		result = stb_vorbis_get_samples_short_interleaved(self->stream, self->info.channels, pcm+size, BUFFER_SIZE-size);
		if(result > 0) size += result*self->info.channels;
		else break;
	}
 
	if(size == 0) return false;
 
	alBufferData(buffer, self->format, pcm, size*sizeof(ALshort), self->info.sample_rate);
	self->totalSamplesLeft-=size;
	#undef BUFFER_SIZE
 
	return true;
}
 
bool AudioStreamOpen(AudioStream* self, const char* filename){
	self->stream = stb_vorbis_open_filename((char*)filename, NULL, NULL);
	if(!self->stream) return false;
	// Get file info
	self->info = stb_vorbis_get_info(self->stream);
	if(self->info.channels == 2) self->format = AL_FORMAT_STEREO16;
	else self->format = AL_FORMAT_MONO16;
 
	if(!AudioStreamStream(self, self->buffers[0])) return false;
	if(!AudioStreamStream(self, self->buffers[1])) return false;
	alSourceQueueBuffers(self->source, 2, self->buffers);
	alSourcePlay(self->source);
 
	self->totalSamplesLeft=stb_vorbis_stream_length_in_samples(self->stream) * self->info.channels;
 
	return true;
}
 
bool AudioStreamUpdate(AudioStream* self){
	ALint processed=0;
 
    alGetSourcei(self->source, AL_BUFFERS_PROCESSED, &processed);
 
    while(processed--){
        ALuint buffer=0;
        
        alSourceUnqueueBuffers(self->source, 1, &buffer);
 
		if(!AudioStreamStream(self, buffer)){
			bool shouldExit=true;
 
			if(self->shouldLoop){
				stb_vorbis_seek_start(self->stream);
				self->totalSamplesLeft=stb_vorbis_stream_length_in_samples(self->stream) * self->info.channels;
				shouldExit=!AudioStreamStream(self, buffer);
			}
 
			if(shouldExit) return false;
		}
		alSourceQueueBuffers(self->source, 1, &buffer);
	} 
	return true;
}
    
    void Audio::playStream(const std::string file_name, const Assets & assets) {
        AudioStream * stream = new AudioStream();
        AudioStreamInit(stream);
        
        
        thread_ = new std::thread(std::thread([](std::string file_name, AudioStream * stream){
            std::string path = "assets/" + file_name;
            if (AudioStreamOpen(stream, path.c_str())){
                std::cout << "Open\n";
            }
            while (AudioStreamUpdate(stream)){
                //std::cout << "Playing\n";
            }    
        }, file_name, stream));
    }
}

#endif
