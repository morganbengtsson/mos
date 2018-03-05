#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <mos/gfx/texture_2d.hpp>
#include <mos/gfx/camera.hpp>
#include <mos/gfx/target.hpp>

namespace mos {
namespace gfx {

/** Spotlight. */
class Light final {
public:
  explicit Light(const glm::vec3 &position = glm::vec3(0.0f, 0.0f, 2.0f),
                 const glm::vec3 &center = glm::vec3(0.0f, 0.0f, 0.0f),
                 const float angle = glm::half_pi<float>(),
                 const glm::vec3 &color = glm::vec3(0.0f),
                 const float strength = 100.0f);
  virtual ~Light();

  /** Set spot angle, in radans. */
  void angle(const float angle);

  float angle() const;

  /** Set position. */
  void position(const glm::vec3 &position);

  glm::vec3 position() const;

  /** Set center/focus point. */
  void center(const glm::vec3 &center);

  glm::vec3 center() const;

  glm::vec3 direction() const;

  glm::vec3 color;

  /** Strength of the lamp in watts. */
  float strength;

  /** Camera for shadow map rendering. */
  Camera camera;

  SharedTexture2D shadow_map;

  /** Target for shadow map rendering. */
  Target target;
private:
  float angle_;
};
}
}

