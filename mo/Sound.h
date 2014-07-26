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

    class Sound {
    public:
        typedef std::vector<char> Samples;
        template<class It>
        Sound(It begin, It end){
            static unsigned int current_id = 0;
            id_ = current_id++;
            samples_.assign(begin, end);
        }
        virtual ~Sound();
        
        Samples::const_iterator begin(){
            return samples_.begin();
        }
        Samples::const_iterator end(){
            return samples_.end();
        }
        
        char * data(){return samples_.data();}
        
        unsigned int id();
        
    private:
        unsigned int id_;
        Samples samples_;

    };
}

#endif	/* MO_SOUND_H */

