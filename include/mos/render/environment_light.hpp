#ifndef MOS_ENVIRONMENT_HPP
#define MOS_ENVIRONMENT_HPP
#include <mos/render/texture_cube.hpp>
#include <mos/render/render_box.hpp>
#include <mos/render/render_target.hpp>
#include <mos/render/render_cube_camera.hpp>
namespace mos {
/**
* @brief Class for environmental lighting.
*
* Contains a texture cube for environmentual lighting, both specular and diffuse.
* The diffuse/blurred lighting is computed by shaders. The box is used for parallax correction,
* so it should match the current lit scene as close as possible.
*/
class EnvironmentLight {
public:
  /**
   * @brief Environment constructor.
   * @param texture Is the environment texture.
   * @param box Describes how big the environment is, for parallax correction.
   */
  EnvironmentLight(const glm::vec3 &position = glm::vec3(0.0f, 0.0f, 1.0f),
                   const glm::vec3 &extent = glm::vec3(50.0f),
                   const float strength = 1.0f);
  EnvironmentLight(const EnvironmentLight &light);
  EnvironmentLight& operator=(const EnvironmentLight &other);

  TextureCube texture;
  RenderBox box;
  RenderTarget target;
  RenderCubeCamera cube_camera;
  float strength;
};
}

#endif //MOS_ENVIRONMENT_HPP
