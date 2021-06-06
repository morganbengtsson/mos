#pragma once
#include <mos/gfx/box.hpp>
#include <mos/gfx/target.hpp>
#include <mos/gfx/cube_camera.hpp>

namespace mos::gfx {

class Environment_light;
using Optional_environment_light = std::optional<Environment_light>;

/** Environment light, based on a cube map. */
class Environment_light final {
public:
  /** @param extent Describes how big the environment is, for parallax/box correction. */
  explicit Environment_light(const glm::vec3 &position = glm::vec3(0.0f, 0.0f, 1.0f),
                   const glm::vec3 &extent = glm::vec3(50.0f),
                   float strength = 0.0f,
                   float falloff = 0.1f,
                   float near = 0.1f);

  static auto load(const std::string &directory, const std::string &path,
                   const glm::mat4 &parent_transform = glm::mat4(1.0f)) -> Environment_light;

  /** Set position. */
  auto position(const glm::vec3 &position) -> void;

  /** Get position. */
  auto position() const -> glm::vec3;

  /** Set extent and recalculate cube camera projections.*/
  auto extent(const glm::vec3 &extent) -> void;

  /** Get extent. */
  auto extent() const -> glm::vec3;

  /** Check if a point is inside the environment light box. */
  auto inside(const glm::vec3 &point) const -> bool;

  /** Get one of the six cube cameras, for rendering. */
  auto camera(size_t index) const -> Camera;

  /** Strength. */
  float strength{0.0f};

  /** Falloff. */
  float falloff{0.1f};
private:
  Box box_;
  Cube_camera cube_camera_;
};
}

