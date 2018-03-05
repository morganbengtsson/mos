#pragma once
#include <mos/gfx/texture_cube.hpp>
#include <mos/gfx/box.hpp>
#include <mos/gfx/target.hpp>
#include <mos/gfx/cube_camera.hpp>

namespace mos {
namespace gfx {
/**
* @brief Class for real time environment light.
*
*/
class EnvironmentLight {
  friend class Renderer;
public:
  /**
   * @brief Environment constructor.
   * @param extent Describes how big the environment is, for parallax correction.
   */
  EnvironmentLight(const glm::vec3 &position = glm::vec3(0.0f, 0.0f, 1.0f),
                   const glm::vec3 &extent = glm::vec3(50.0f),
                   const float strength = 1.0f,
                   const glm::uvec2 resolution = glm::uvec2(128, 128));
  EnvironmentLight(const EnvironmentLight &light);
  EnvironmentLight &operator=(const EnvironmentLight &other);

  Box box;
  float strength;

private:
  Target target;
  CubeCamera cube_camera;
  TextureCube texture;
};
}
}
