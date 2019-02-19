#pragma once

#include <glm/glm.hpp>
#include <array>
#include <map>

namespace mos {
namespace gfx {

/** Rendering camera view. */
class Camera final {
public:
  Camera(const glm::vec3 &position = glm::vec3(0.0f),
               const glm::vec3 &center = glm::vec3(0.0f),
               const glm::mat4 &projection = glm::mat4(1.0f),
               const glm::vec3 &up = glm::vec3(0.0f, 0.0f, 1.0f));
  ~Camera() = default;

  glm::vec3 up() const;

  /** Set up vector. */
  void up(const glm::vec3 &up);

  glm::vec3 position() const;

  /** Set position. */
  void position(const glm::vec3 &position);

  glm::vec3 center() const;

  /** Set center/focus point. */
  void center(const glm::vec3 &center);

  glm::vec3 direction() const;

  glm::vec3 right() const;

  /** Set direction. */
  void direction(const glm::vec3 &direction);

  float aspect_ratio() const;

  bool in_frustum(const glm::vec3 & point, const float radius) const;

  glm::mat4 projection;
  glm::mat4 view;

private:
  void calculate_view();
  void calculate_frustum();
  std::array<glm::vec4, 6> frustum_planes_;
  glm::vec3 up_;
  glm::vec3 center_;
  glm::vec3 position_;
};
}
}
