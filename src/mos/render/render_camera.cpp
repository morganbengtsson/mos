#include <mos/render/render_camera.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace mos {
  RenderCamera::RenderCamera() {
  }

  RenderCamera::RenderCamera(const glm::vec3 &position,
                   const glm::vec3 &center,
                   const glm::mat4 &projection,
                   const glm::vec3 &up,
                   const glm::vec2 &resolution)
    : projection(projection), view(glm::lookAt(position, center, up)), up_(up),
      position_(position), center_(center), resolution(resolution) {}

glm::vec3 RenderCamera::up() const { return up_; }

void RenderCamera::up(const glm::vec3 &up) {
  up_ = up;
  update_view();
}

glm::vec3 RenderCamera::position() const { return position_; }

void RenderCamera::position(const glm::vec3 &position) {
  position_ = position;
  update_view();
}

glm::vec3 RenderCamera::center() const { return center_; }

void RenderCamera::center(const glm::vec3 &center) {
  center_ = center;
  update_view();
}

glm::vec3 RenderCamera::direction() const {
  return glm::normalize(center_ - position_);
}

void RenderCamera::update_view() { view = glm::lookAt(position_, center_, up_); }

float RenderCamera::aspect_ratio() const {
  return projection[1][1] / projection[0][0];
}
}
