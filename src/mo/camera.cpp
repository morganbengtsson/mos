#include "camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace mo {
    Camera::Camera(const glm::vec3 &position,
                   const glm::vec3 &center,
                   const glm::mat4 &projection,
                   const glm::vec3 &up) :
            projection(projection),
            view(glm::lookAt(position, center, up)),
            up_(up),
            position_(position),
            center_(center) {

    }

    glm::vec3 Camera::position() const {
        return position_;
    }

    void Camera::position(glm::vec3 position) {
        position_ = position;
        view = glm::lookAt(position_, center_, up_);
    }
}
