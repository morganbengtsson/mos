#include <mos/gfx/camera.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/io.hpp>
#include <iostream>

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
  calculate_view();
  calculate_frustum();
}

glm::vec3 Camera::up() const { return up_; }

void Camera::up(const glm::vec3 &up) {
  up_ = up;
  calculate_view();
  calculate_frustum();
}

glm::vec3 Camera::position() const { return position_; }

void Camera::position(const glm::vec3 &position) {
  position_ = position;
  calculate_view();
  calculate_frustum();
}

glm::vec3 Camera::center() const { return center_; }

void Camera::center(const glm::vec3 &center) {
  center_ = center;
  calculate_view();
  calculate_frustum();
}

glm::vec3 Camera::direction() const {
  return glm::normalize(center_ - position_);
}

void Camera::direction(const glm::vec3 &direction) {
  center(position_ + direction);
}

void Camera::calculate_view() {
  view = glm::lookAt(position_, center_, up_);
}

float Camera::aspect_ratio() const {
  return projection[1][1] / projection[0][0];
}

void Camera::calculate_frustum() {

  auto m = projection * view;
  auto t = glm::transpose(m);

  frustum_planes_[0] = t[3] + t[0]; /* left   */
  frustum_planes_[1] = t[3] - t[0]; /* right  */
  frustum_planes_[2] = t[3] + t[1]; /* bottom */
  frustum_planes_[3] = t[3] - t[1]; /* top    */
  frustum_planes_[4] = t[3] + t[2]; /* near   */
  frustum_planes_[5] = t[3] - t[2]; /* far    */

  /*
  // Calculate the LEFT side
  frustum_planes_[0].x = m[0][3] + m[0][0];
  frustum_planes_[0].y = m[1][3] + m[1][0];
  frustum_planes_[0].z = m[2][3] + m[2][0];
  frustum_planes_[0].w = m[3][3] + m[3][0];

  // Calculate the RIGHT side
  frustum_planes_[1].x = m[0][3] - m[0][0];
  frustum_planes_[1].y = m[1][3] - m[1][0];
  frustum_planes_[1].z = m[2][3] - m[2][0];
  frustum_planes_[1].w = m[3][3] - m[3][0];

  // Calculate the TOP side
  frustum_planes_[2].x = m[0][3] - m[0][1];
  frustum_planes_[2].y = m[1][3] - m[1][1];
  frustum_planes_[2].z = m[2][3] - m[2][1];
  frustum_planes_[2].w = m[3][3] - m[3][1];

  // Calculate the BOTTOM side
  frustum_planes_[3].x = m[0][3] + m[0][1];
  frustum_planes_[3].y = m[1][3] + m[1][1];
  frustum_planes_[3].z = m[2][3] + m[2][1];
  frustum_planes_[3].w = m[3][3] + m[3][1];

  // Calculate the FRONT side
  frustum_planes_[4].x = m[0][3] + m[0][2];
  frustum_planes_[4].y = m[1][3] + m[1][2];
  frustum_planes_[4].z = m[2][3] + m[2][2];
  frustum_planes_[4].w = m[3][3] + m[3][2];

  // Calculate the BACK side
  frustum_planes_[5].x = m[0][3] - m[0][2];
  frustum_planes_[5].y = m[1][3] - m[1][2];
  frustum_planes_[5].z = m[2][3] - m[2][2];
  frustum_planes_[5].w = m[3][3] - m[3][2];
 */

  //Normalize planes
  for (int i = 0; i < frustum_planes_.size(); i++) {
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

