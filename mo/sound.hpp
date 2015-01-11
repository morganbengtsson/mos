/* 
 * File:   Sound.h
 * Author: morgan
 *
 * Created on May 6, 2014, 9:38 PM
 */

#ifndef MO_SOUND_H
#define	MO_SOUND_H

#include <vector>

namespace mo {

    /*!
     * A sound class containing samples of 16bit integer data.
     */
    class Sound {
    public:
        using Samples = std::vector<short>;
        template<class It>
        Sound(It begin, It end, const unsigned int channels = 1u) :
            channels_(channels),
            valid(true),
            samples_(begin, end) {
            static unsigned int current_id = 0;
            id_ = current_id++;
        }
        Sound(const unsigned int channels = 1u): channels_(channels){
            static unsigned int current_id = 0;
            id_ = current_id++;
        }

        virtual ~Sound();
        
        Samples::const_iterator begin() const {
            return samples_.begin();
        }
        Samples::const_iterator end() const {
            return samples_.end();
        }
        
        const short * data() const;
        unsigned int id() const;
        bool valid;
        unsigned int channels() const;

    private:
        unsigned int id_;
        Samples samples_;
        unsigned int channels_;


    };
}

#endif	/* MO_SOUND_H */

