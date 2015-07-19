#include "camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace mos {
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

    void Camera::position(const glm::vec3 & position) {
        position_ = position;
        view = glm::lookAt(position_, center_, up_);
    }

    glm::vec3 Camera::center() const {
        return center_;
    }

    void Camera::center(const glm::vec3 & center) {
        center_ = center;
        view = glm::lookAt(position_, center_, up_);
    }
}
