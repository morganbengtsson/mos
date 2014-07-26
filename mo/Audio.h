/* 
 * File:   Audio.h
 * Author: morgan
 *
 * Created on May 7, 2014, 9:41 PM
 */

#ifndef MO_AUDIO_H
#define	MO_AUDIO_H

#ifdef __ANDROID__
#include <android_native_app_glue.h>
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

#include "Sound.h"

namespace mo {

    class Audio {
    public:
        Audio();
        virtual ~Audio();

        void play(const std::shared_ptr<Sound> sound);

        void setListenerPosition(glm::vec3 position);
        void setListenerVelocity(glm::vec3 velocity);
        void setListenerOrientation(glm::vec3 orientation);

    private:
#ifdef __ANDROID__
        void startPlayer();
        void stop();
        // OpenSL ES engine.
	SLObjectItf engine_obj_; SLEngineItf engine_;
	// Audio output.
	SLObjectItf output_mix_obj_;

	// Background music player.
	SLObjectItf BGM_player_obj_; SLPlayItf BGM_player_;
	SLSeekItf BGM_player_seek_;
        
        // Sound player.
	SLObjectItf player_obj_; SLPlayItf player_;
	SLBufferQueueItf player_queue_;
#else
        ALCdevice* device_;
        ALCcontext* context_;

        std::map<unsigned int, ALuint> sources_;
        std::map<unsigned int, ALuint> buffers_;
#endif
    };
}

#endif	/* MO_AUDIO_H */

