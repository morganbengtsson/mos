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
                 const float strength = 100.0f,
                 const float near = 0.1f,
                 const float far = 10.0f);

  Light(const std::string &directory, const std::string &path, const float near = 0.1f, const float far = 100.0f,
      const glm::mat4 &parent_transform = glm::mat4(1.0f));

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

  /** Set near and far plane. **/
  void near_far(const float near, const float far);

  glm::vec3 color;

  /** Strength of the lamp in watts. */
  float strength;

  /** Camera for shadow map rendering. */
  Camera camera;

private:
  float angle_;
  float near_;
  float far_;
};
}
}

