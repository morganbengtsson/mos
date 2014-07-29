/* 
 * File:   Source.h
 * Author: morgan
 *
 * Created on July 28, 2014, 3:47 PM
 */

#ifndef SOURCE_H
#define	SOURCE_H

#include <vector>
#include <memory>
#include <initializer_list>
#include <glm/glm.hpp>
#include "Sound.h"

namespace mo {

    /*!
     * A classed used for audio playback. Contains sounds, together with a position.
     * That is used if 3D audio is enabled.
     */
    class Source {
    public:
        typedef std::vector<std::shared_ptr<Sound>> Sounds;
        
        template<class It>
        Source(It begin, It end, const glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f)): position(position){
            static unsigned int current_id = 0;
            id_ = current_id++;
            sounds_.assign(begin, end);
        }
        template<class It>
        Source(std::initializer_list<It> il, const glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f)) : Source(il.begin(), il.end(), position){
            
        } 
        virtual ~Source();
        
        glm::vec3 position;
        
        const Sounds::const_iterator begin() const;
        const Sounds::const_iterator end() const;
        unsigned int id() const;
    private:
        unsigned int id_;
        Sounds sounds_;
    };

}

#endif	/* SOURCE_H */

