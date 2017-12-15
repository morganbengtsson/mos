#ifndef MOS_ENVIRONMENT_HPP
#define MOS_ENVIRONMENT_HPP
#include <mos/render/texture_cube.hpp>
#include <mos/render/render_box.hpp>
namespace  mos {
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
  EnvironmentLight(const SharedTextureCube & texture = nullptr,
              const RenderBox &box = RenderBox(glm::vec3(0.0f), glm::vec3(50.0f)));
  SharedTextureCube texture;
  RenderBox box;
};
}

#endif //MOS_ENVIRONMENT_HPP
