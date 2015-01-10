/* 
 * File:   Audio.h
 * Author: morgan
 *
 * Created on May 7, 2014, 9:41 PM
 */

#ifndef MO_AUDIO_H
#define	MO_AUDIO_H

#ifdef __ANDROID__
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <SLES/OpenSLES_AndroidConfiguration.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#endif


#include <stb_vorbis.h>
#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <future>

#include "sound.hpp"
#include "source.hpp"
#include "assets.hpp"
#include "streamsource.hpp"

namespace mo {
    
#ifdef __ANDROID__
    struct Player {
        SLObjectItf player_obj_;
        SLPlayItf player_;
        SLBufferQueueItf player_queue_;
    };

#endif

    /*!
     * Audio player class. Uses OpenAL for Windows/Linux/OSX and OpenSL for Android.
     */
    class Audio {
    public:
        Audio();
        virtual ~Audio();

        /**
         * Play audio from a source object. A source may contain many sound objects.
         * 
         * @param source
         */
        void play(const Source & source);
        void play(const StreamSource & stream_source);
        void update(const StreamSource & stream_source);
        void stop();
        glm::vec3 listener_position();
        void listener_position(const glm::vec3 position);
        glm::vec3 listener_orientation();
        void listener_orientation(const glm::vec3 orientation, const glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f));
        glm::vec3 listener_velocity();
        void listener_velocity(const glm::vec3 velocity);
    private:
#ifdef __ANDROID__
        void createBufferPlayers();        
        
        // OpenSL ES engine.
        SLObjectItf engine_obj_;
        SLEngineItf engine_;
        
        // Audio outputmix.
        SLObjectItf output_mix_obj_;

        // Background music player.
        SLObjectItf descriptor_player_obj_;
        SLPlayItf descriptor_player_;
        SLSeekItf descriptor_player_seek_;

        // Sound players.
        std::vector<Player> players_;
        int latest_player;
        
#else 

        ALCdevice* device_;
        ALCcontext* context_;

        using SourcePair = std::pair<unsigned int, ALuint>;
        using BufferPair = std::pair<unsigned int, ALuint>;
        using Sources = std::map<unsigned int, ALuint>;
        using Buffers = std::map<unsigned int, ALuint>;     

        Sources sources_;
        Buffers buffers_;        
        std::vector<std::thread> stream_threads;

#endif
    };
}

#endif	/* MO_AUDIO_H */

