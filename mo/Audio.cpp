/* 
 * File:   Audio.cpp
 * Author: morgan
 * 
 * Created on May 7, 2014, 9:41 PM
 */

#ifdef __ANDROID__

#else

#include "Audio.h"
namespace mo {

    Audio::Audio() {
        ALCint contextAttr[] = {ALC_FREQUENCY, 44100, 0};
        device_ = alcOpenDevice(NULL);
        context_ = alcCreateContext(device_, contextAttr);
        alcMakeContextCurrent(context_);

        setListenerPosition(glm::vec3(0.0f, 0.0f, 0.0f));
        setListenerVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
        setListenerOrientation(glm::vec3(0.0f, 0.0f, -1.0f));
    }

    Audio::~Audio() {
    }

    void Audio::play(const std::shared_ptr<Sound> sound){
        
        ALuint source;
        alGenSources( 1, &source);
        alSourcef( source, AL_PITCH, 1. );
        alSourcef( source, AL_GAIN, 1. );        
        alSource3f( source, AL_POSITION, 0.0f, 0.0f, 0.0f );        
        alSource3f( source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
        alSourcei( source, AL_LOOPING, AL_TRUE );
        
        ALuint buffer;
        
        alGenBuffers( 1, &buffer );
        {
           long dataSize = std::distance(sound->begin(), sound->end());
           const ALvoid* data = sound->data();           
           alBufferData( buffer, AL_FORMAT_MONO16, data, dataSize, 44100 );
           free( (void*)data );
        }
        alSourcei( source, AL_BUFFER, buffer );
        
        alSourcePlay( source );
    }
    
    void Audio::setListenerPosition(glm::vec3 position) {
        alListener3f(AL_POSITION, position.x, position.y, position.z);
    }

    void Audio::setListenerVelocity(glm::vec3 velocity) {
        alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);
    }

    void Audio::setListenerOrientation(glm::vec3 orientation) {
        float orient[6] = {/*fwd:*/ orientation.x, orientation.y, orientation.z, /*up:*/ 0., 1., 0.};
        alListenerfv(AL_ORIENTATION, orient);
    }

}

#endif