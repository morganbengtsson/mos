/* 
 * File:   Source.h
 * Author: morgan
 *
 * Created on July 28, 2014, 3:47 PM
 */

#ifndef SOURCE_H
#define	SOURCE_H

#include <glm/vec3.hpp>

namespace mo {

/*!
 * Audio source base
 */
class Source {
public:
    Source(const glm::vec3 position = glm::vec3(0.0f),
           const glm::vec3 velocity = glm::vec3(0.0f),
           const float pitch = 1.0f,
           const float gain = 1.0f,
           bool loop = false,
           bool playing = true);
    virtual ~Source();
    unsigned int id() const;
    glm::vec3 position;
    glm::vec3 velocity;
    float pitch;
    float gain;
    bool loop;
    bool playing;
private:
    unsigned int id_;

};

}

#endif	/* SOURCE_H */

