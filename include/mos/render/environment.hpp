#ifndef MOS_ENVIRONMENT_HPP
#define MOS_ENVIRONMENT_HPP
#include <mos/render/texture_cube.hpp>
#include <mos/render/render_box.hpp>
namespace  mos {
class Environment {
public:
  Environment(const SharedTextureCube & texture = nullptr,
              const RenderBox &box = RenderBox(glm::vec3(0.0f), glm::vec3(50.0f)));
  SharedTextureCube texture;
  RenderBox box;
};
}

#endif //MOS_ENVIRONMENT_HPP
