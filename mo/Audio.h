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

#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <future>

#include "Sound.h"
#include "Source.h"
#include "Assets.h"

namespace mo {

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
        void playStream(const std::string file_name, const Assets & assets);

    private:
#ifdef __ANDROID__
        void createBufferPlayer();        
        void stop();
        // OpenSL ES engine.
        SLObjectItf engine_obj_;
        SLEngineItf engine_;
        
        // Audio outputmix.
        SLObjectItf output_mix_obj_;

        // Background music player.
        SLObjectItf descriptor_player_obj_;
        SLPlayItf descriptor_player_;
        SLSeekItf descriptor_player_seek_;

        // Sound player.
        SLObjectItf player_obj_;
        SLPlayItf player_;
        SLBufferQueueItf player_queue_;
#else
        ALCdevice* device_;
        ALCcontext* context_;

        typedef std::pair<unsigned int, ALuint> SourcePair;
        typedef std::pair<unsigned int, ALuint> BufferPair;
        typedef std::map<unsigned int, ALuint> Sources;
        typedef std::map<unsigned int, ALuint> Buffers;

        Sources sources_;
        Buffers buffers_;
        
        std::thread * thread_;
        
#endif
    };
}

#endif	/* MO_AUDIO_H */

