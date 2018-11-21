#include <mos/gfx/camera.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/intersect.hpp>

namespace mos {
namespace gfx {

Camera::Camera() {
}

Camera::Camera(const glm::vec3 &position,
                 const glm::vec3 &center,
                 const glm::mat4 &projection,
                 const glm::vec3 &up)
  : projection(projection), up_(up),
    position_(position), center_(center){
  calculate_view_and_frustum();
}

glm::vec3 Camera::up() const { return up_; }

void Camera::up(const glm::vec3 &up) {
  up_ = up;
  calculate_view_and_frustum();
}

glm::vec3 Camera::position() const { return position_; }

void Camera::position(const glm::vec3 &position) {
  position_ = position;
  calculate_view_and_frustum();
}

glm::vec3 Camera::center() const { return center_; }

void Camera::center(const glm::vec3 &center) {
  center_ = center;
  calculate_view_and_frustum();
}

glm::vec3 Camera::direction() const {
  return glm::normalize(center_ - position_);
}

void Camera::direction(const glm::vec3 &direction) {
  center(position_ + direction);
  calculate_view_and_frustum();
}


void Camera::calculate_view_and_frustum() {
  view = glm::lookAt(position_, center_, up_);
  calculate_frustum_planes();
}

float Camera::aspect_ratio() const {
  return projection[1][1] / projection[0][0];
}

void Camera::calculate_frustum_planes() {

    auto m = projection * view;

    auto t = glm::transpose(m);

    frustum_planes_[0] = t[3] + t[0]; /* left   */
    frustum_planes_[1] = t[3] - t[0]; /* right  */
    frustum_planes_[3] = t[3] + t[1]; /* bottom */
    frustum_planes_[4] = t[3] - t[1]; /* top    */
    frustum_planes_[5] = t[3] + t[2]; /* near   */
    frustum_planes_[6] = t[3] - t[2]; /* far    */

    //Normalize planes
    for (int i = 0; i++; i < frustum_planes_.size()) {
      float mag = glm::length(glm::vec3(frustum_planes_[i]));
      frustum_planes_[i] /= mag;
    }
  }
bool Camera::in_frustum(const glm::vec3 &point, const float radius) const {

    bool result = true;

    for(int i=0; i < 6; i++) {
      auto plane_normal = glm::vec3(frustum_planes_[i]);
      auto plane_point = plane_normal * frustum_planes_[i].w;
      float distance = glm::dot(plane_normal, point - plane_point);

      if (distance < -radius)
        return false;
      else if (distance < radius)
        result = true;
    }
    return result;
}

}
}

