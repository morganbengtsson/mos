#ifndef MOS_AUDIO_H
#define	MOS_AUDIO_H

#include <unordered_map>
#include <memory>
#include <thread>

#include "AL/al.h"
#include "AL/alc.h"
#include "AL/alext.h"
#include "AL/efx-presets.h"

#include "streamsource.hpp"
#include "soundsource.hpp"

namespace mos {    

    /**
     * Audio player class. Uses OpenAL for Windows/Linux/OSX.
     */
    class Player {
    public:
        Player();

        /**
         * @brief Audio class is non copyable.
         * @param audio
         */
        Player(const Player & audio) = delete; //Do not copy
        virtual ~Player();

        template<class It>
        /**
         * A generalized init method
         *
         * @brief init
         * @param begin
         * @param end
         */
        void init(It begin, It end) {
            for (auto it = begin; it != end; it++){
                init(*it);
            }
        }

        /**
         * Play audio from a SoundSource object. A sound source contains one sound.
         * 
         * @param source
         */
        void init(const SoundSource & source);

        /**
         * Plays audio from a StreamSource. Which streams content from a file. This
         * method starts a new thread for each source playing.
         *
         * @brief play
         * @param stream_source
         */
        void init(const StreamSource & stream_source);

        template<class It>
        /**
         * @brief A generalized update method
         * @param begin Begin iterator.
         * @param end End iterator.
         * @param dt Time step.
         */
        void update(It begin, It end, const float dt) {
            for (auto it = begin; it != end; it++){
                update(*it, dt);
            }
        }

        /**
         * Updates the internal source representation with data. Data
         * such as position, velocity, pitch and gain.
         *
         * @brief update
         * @param sound_source
         * @param dt Time step.
         */
        void update(SoundSource & sound_source, const float dt);

		/**
		* Updates the internal source representation with data. Data
		* such as position, velocity, pitch and gain.
		*
		* @brief update
		* @param stream_source
        * @param dt Time step.
		*/
        void update(StreamSource & stream_source, const float dt);

        /**
         * @brief listener_position
         * @return Position of the listener.
         */
        glm::vec3 listener_position();

		/**
		* Set the listener position. 
		*
		* @brief listener_position
		* @param position
		*/
        void listener_position(const glm::vec3 position);

		/**
		* Get the listener position
		*
		* @brief listener_position	
		*/
        glm::vec3 listener_orientation();

		/**
		* Set the listener orientation.
		*
		* @brief listener_orientation
		* @param orientation, up
		*/
        void listener_orientation(const glm::vec3 orientation, const glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f));
        
		/**
		* Get the listener velocity.
		*
		* @brief listener_velocity
		*/
		glm::vec3 listener_velocity();

		/**
		* Set the listener velocity.
		*
		* @brief listener_velocity
		* @param velocity
		*/
        void listener_velocity(const glm::vec3 velocity);
    private:
		struct StreamThread {
			std::shared_ptr<std::thread> thread;
			bool running;
		};

        ALCdevice * device_;
        ALCcontext * context_;

        EFXEAXREVERBPROPERTIES reverb_properties;
        ALuint reverb_effect;
        ALuint reverb_slot;

        ALuint lowpass_filter1;
        ALuint lowpass_filter2;

        using SourcePair = std::pair<unsigned int, ALuint>;
        using BufferPair = std::pair<unsigned int, ALuint>;
        using Sources = std::unordered_map<unsigned int, ALuint>;
        using Buffers = std::unordered_map<unsigned int, ALuint>;
        using Filters = std::unordered_map<unsigned int, ALuint>;

        Sources sources_;
        Buffers buffers_;
        Filters filters_;

        std::unordered_map<unsigned int, StreamThread> stream_threads;		
    };
}

#endif	/* MOS_AUDIO_H */

