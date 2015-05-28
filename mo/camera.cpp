#include "camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace mo {
    Camera::Camera(const glm::vec3 &eye,
                   const glm::vec3 &center,
                   const glm::mat4 &projection,
                   const glm::vec3 &up) :
            projection(projection), view(glm::lookAt(eye, center, up)) {

    }
}