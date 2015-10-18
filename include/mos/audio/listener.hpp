#ifndef MOS_LISTENER_H
#define	MOS_LISTENER_H

#include <glm/glm.hpp>

namespace mos {

class Listener {
public:
    Listener(const glm::vec3 & position = glm::vec3(0.0f),
             const glm::vec3 & velocity = glm::vec3(0.0f),
             const glm::vec3 & orientation = glm::vec3(0.0f, 1.0f, 0.0f));
    ~Listener();
    glm::vec3 position;
    glm::vec3 orientation;
    glm::vec3 velocity;
};

}

#endif	/* MOS_LISTENER_H */
