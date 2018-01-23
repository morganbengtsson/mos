#ifndef MOS_ENVIRONMENT_HPP
#define MOS_ENVIRONMENT_HPP
#include <mos/render/texture_cube.hpp>
#include <mos/render/render_box.hpp>
#include <mos/render/render_target.hpp>
#include <mos/render/render_cube_camera.hpp>
namespace mos {
/**
* @brief Class for real time environment light.
*
*/
class EnvironmentLight {
  friend class RenderSystem;
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

  RenderBox box;
  float strength;

private:
  RenderTarget target;
  RenderCubeCamera cube_camera;
  TextureCube texture;
};
}

#endif //MOS_ENVIRONMENT_HPP
