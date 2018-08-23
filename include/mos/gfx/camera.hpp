#pragma once

#include <glm/glm.hpp>

namespace mos {
namespace gfx {

/** Rendering camera view. */
class Camera final {
public:
  Camera();
  Camera(const glm::vec3 &position,
               const glm::vec3 &center,
               const glm::mat4 &projection,
               const glm::vec3 &up = glm::vec3(0.0f, 0.0f, 1.0f));

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

  /** Set direction. */
  void direction(const glm::vec3 &direction);

  float aspect_ratio() const;

  glm::mat4 projection;
  glm::mat4 view;

private:
  void update_view();
  glm::vec3 up_;
  glm::vec3 center_;
  glm::vec3 position_;
};
}
}