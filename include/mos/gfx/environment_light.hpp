#pragma once
#include <mos/gfx/box.hpp>
#include <mos/gfx/target.hpp>
#include <mos/gfx/cube_camera.hpp>

namespace mos {
namespace gfx {

class EnvironmentLight;
using OptionalEnvironmentLight = std::optional<EnvironmentLight>;

/** Environment light, based on a cube map. */
class EnvironmentLight final {
  friend class Renderer;
public:
  /** @param extent Describes how big the environment is, for parallax/box correction. */
  explicit EnvironmentLight(const glm::vec3 &position = glm::vec3(0.0f, 0.0f, 1.0f),
                   const glm::vec3 &extent = glm::vec3(50.0f),
                   const float strength = 0.0f);

  EnvironmentLight(const std::string &directory, const std::string &path,
      const glm::mat4 &parent_transform = glm::mat4(1.0f));

  /** Set position. */
  void position(const glm::vec3 &position);

  /** Get position. */
  glm::vec3 position() const;

  /** Set extent and recalculate cube camera projections.*/
  void extent(const glm::vec3 &extent);

  /** Get extent. */
  glm::vec3 extent() const;

  /** Check if a point is inside the environment light box. */
  bool inside(const glm::vec3 &point) const;

  /** Strength. */
  float strength;
private:
  Box box_;
  CubeCamera cube_camera_;
};
}
}
