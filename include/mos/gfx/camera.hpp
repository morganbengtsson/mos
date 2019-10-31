#pragma once

#include <glm/glm.hpp>
#include <array>
#include <map>

namespace mos {
namespace gfx {

/** Rendering camera view. */
class Camera final {
public:
  using Planes = std::array<glm::vec4, 6>;
  Camera(const glm::vec3 &position = glm::vec3(0.0f),
               const glm::vec3 &center = glm::vec3(0.0f),
               const glm::mat4 &projection = glm::mat4(1.0f),
               const glm::vec3 &up = glm::vec3(0.0f, 0.0f, 1.0f),
               float fstop = 1.0f);

  Camera(const glm::vec3 &position,
         const glm::mat4 &projection,
         const float &focus_distance,
         const glm::vec3 &up = glm::vec3(0.0f, 0.0f, 1.0f),
         float fstop = 1.0f);

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

  /** Check if sphere with a radius is within camera frustum. */
  bool in_frustum(const glm::vec3 & point, float radius) const;
  
  /** Get near clip plane. */
  float near_plane() const;

  /** Get far clip plane. */
  float far_plane() const;

  /** Get projection matrix. */
  glm::mat4 projection() const;

  /** Set projection matrix. */
  void projection(const glm::mat4 &proj);

  /** Get view matrix */
  glm::mat4 view() const;

  /** Camera fstop for depth of field. */
  float fstop;

private:
  void calculate_view();
  void calculate_frustum();
  void calculate_near_far();
  glm::mat4 projection_;
  glm::mat4 view_;
  Planes frustum_planes_;
  glm::vec3 up_;
  glm::vec3 center_;
  glm::vec3 position_;
  float near_{0.1f};
  float far_{100.0f};
};
}
}
