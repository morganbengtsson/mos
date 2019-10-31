#include <mos/gfx/camera.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/io.hpp>
#include <iostream>

namespace mos {
namespace gfx {

Camera::Camera(const glm::vec3 &position, const glm::vec3 &center,
               const glm::mat4 &projection, const glm::vec3 &up,
               const float fstop)
    : fstop(fstop), projection_(projection),
      view_(1.0f), frustum_planes_{glm::vec4(0.0f), glm::vec4(0.0f),
                                   glm::vec4(0.0f), glm::vec4(0.0f),
                                   glm::vec4(0.0f), glm::vec4(0.0f)},
      up_(up), center_(center), position_(position) {
  calculate_view();
  calculate_frustum();
  calculate_near_far();
}

Camera::Camera(const glm::vec3 &position, const glm::mat4 &projection,
               const float &focus_distance, const glm::vec3 &up, float fstop)
    : fstop(fstop), projection_(projection),
      view_(1.0f), frustum_planes_{glm::vec4(0.0f), glm::vec4(0.0f),
                                   glm::vec4(0.0f), glm::vec4(0.0f),
                                   glm::vec4(0.0f), glm::vec4(0.0f)},
      up_(up), center_(focus_distance * direction()), position_(position) {
  calculate_view();
  calculate_frustum();
  calculate_near_far();
}

glm::vec3 Camera::up() const { return up_; }

void Camera::up(const glm::vec3 &up) {
  up_ = up;
  calculate_view();
  calculate_frustum();
  calculate_near_far();
}

glm::vec3 Camera::position() const { return position_; }

void Camera::position(const glm::vec3 &position) {
  position_ = position;
  calculate_view();
  calculate_frustum();
  calculate_near_far();
}

glm::vec3 Camera::center() const { return center_; }

void Camera::center(const glm::vec3 &center) {
  center_ = center;
  calculate_view();
  calculate_frustum();
  calculate_near_far();
}

glm::vec3 Camera::direction() const {
  return glm::normalize(center_ - position_);
}

glm::vec3 Camera::right() const {
  return glm::normalize(glm::cross(up(), direction()));
}

void Camera::direction(const glm::vec3 &direction) {
  center(position_ + direction);
}

void Camera::calculate_view() {
  view_ = glm::lookAt(position_, center_, up_);
}

float Camera::aspect_ratio() const {
  return projection_[1][1] / projection_[0][0];
}

void Camera::calculate_frustum() {
  auto m = projection_ * view_;
  auto t = glm::transpose(m);

  frustum_planes_[0] = t[3] + t[0]; /* left   */
  frustum_planes_[1] = t[3] - t[0]; /* right  */
  frustum_planes_[2] = t[3] + t[1]; /* bottom */
  frustum_planes_[3] = t[3] - t[1]; /* top    */
  frustum_planes_[4] = t[3] + t[2]; /* near   */
  frustum_planes_[5] = t[3] - t[2]; /* far    */

  for (auto & plane : frustum_planes_){
    auto magnitude = glm::length(glm::vec3(plane));
    plane /= magnitude;
  }
}

void Camera::calculate_near_far() {
  auto m = glm::transpose(projection_);

  auto n0 = m[3] + m[2];
  n0 = n0 / glm::length(glm::vec3(n0));
  auto f0 = m[3] - m[2];
  f0 = f0 / glm::length(glm::vec3(f0));

  near_ = n0.z * n0.w;
  far_ = f0.z * f0.w;
}

bool Camera::in_frustum(const glm::vec3 &point, const float radius) const {
  bool result = true;
  for (Planes::size_type i = 0; i < 6; i++)
  {
    if (frustum_planes_[i].x * point.x + frustum_planes_[i].y * point.y + frustum_planes_[i].z * point.z + frustum_planes_[i].w <= -radius) {
      result = false;
    }
  }
  return result;
}

float Camera::near_plane() const {
  return near_;
}

float Camera::far_plane() const {
  return far_;
}

glm::mat4 Camera::projection() const {
  return projection_;
}

void Camera::projection(const glm::mat4 &proj) {
  projection_ = proj;
  calculate_view();
  calculate_frustum();
  calculate_near_far();
}

glm::mat4 Camera::view() const {
  return view_;
}

}
}

