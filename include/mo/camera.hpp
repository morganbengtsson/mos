#ifndef MO_CAMERA_H
#define MO_CAMERA_H

#include <glm/glm.hpp>

namespace mo {

    class Camera {
    public:
        Camera(const glm::vec3 &eye,
               const glm::vec3 &center,
               const glm::mat4 &projection,
               const glm::vec3 &up = glm::vec3(0.0f, 0.0f, 1.0f));

        glm::mat4 projection;
        glm::mat4 view;
    };
}

#endif //MO_CAMERA_H
