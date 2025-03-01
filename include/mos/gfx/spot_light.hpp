#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <mos/gfx/camera.hpp>
#include <mos/gfx/target.hpp>
#include <mos/gfx/texture_2d.hpp>

namespace mos::gfx {

/** Spotlight. */
class Spot_light final {
public:
  explicit Spot_light(const glm::vec3 &position = glm::vec3(0.0F, 0.0F, 2.0F),
                      const glm::vec3 &center = glm::vec3(0.0F, 0.0F, 0.0F),
                      float angle = glm::half_pi<float>(),
                      const glm::vec3 &color = glm::vec3(0.0F),
                      float strength = 0.0F, float near = 0.1F,
                      float far = 10.0F, float blend = 0.15F);

  static auto load(const std::string &directory, const std::string &path,
                   const glm::mat4 &parent_transform = glm::mat4(1.0F))
      -> Spot_light;

  /** Set blend factor. */
  auto blend(float blend) -> void;

  auto blend() const -> float;

  /** Set spot angle, in radans. */
  auto angle(float angle) -> void;

  auto angle() const -> float;

  /** Set position. */
  auto position(const glm::vec3 &position) -> void;

  auto position() const -> glm::vec3;

  /** Set center/focus point. */
  auto direction(const glm::vec3 &direction) -> void;

  auto direction() const -> glm::vec3;

  /** Set near and far plane. **/
  auto near_far(float near, float far) -> void;

  glm::vec3 color = glm::vec3(0.0F);

  /** Strength of the lamp in watts. */
  float strength;

  /** Camera for shadow map rendering. */
  Camera camera;

private:
  float angle_;
  float near_;
  float far_;
  float blend_ = 0.15;
};
} // namespace mos::gfx
