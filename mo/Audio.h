/* 
 * File:   Audio.h
 * Author: morgan
 *
 * Created on May 7, 2014, 9:41 PM
 */

#ifndef MO_AUDIO_H
#define	MO_AUDIO_H

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>

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
        ALCdevice* device_;
        ALCcontext* context_;

        std::map<unsigned int, ALuint> sources_;
        std::map<unsigned int, ALuint> buffers_;
    };
}

#endif	/* MO_AUDIO_H */

