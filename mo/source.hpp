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
#include <glm/glm.hpp>
#include "sound.hpp"
#include "stream.hpp"

namespace mo {

/*!
 * A classed used for audio playback. Contains sounds, together with a position.
 * That is used if 3D audio is enabled.
 */
class Source {
public:

    Source(const std::shared_ptr<Sound> sound,
           const glm::vec3 position = glm::vec3(0.0f),
           const bool loop = false);
    virtual ~Source();
    glm::vec3 position;
    bool loop;
    std::shared_ptr<Sound> sound;
    unsigned int id() const;
private:
    unsigned int id_;


};

}

#endif	/* SOURCE_H */

