#ifndef MOS_RENDER_SCENE_HPP
#define MOS_RENDER_SCENE_HPP

#include <vector>
#include <initializer_list>
#include <mos/render/model.hpp>
#include <mos/render/render_camera.hpp>
#include <mos/render/light.hpp>
#include <mos/render/fog.hpp>
#include <mos/render/render_target.hpp>
#include <mos/render/texture_cube.hpp>
#include <mos/render/render_box.hpp>
#include <mos/render/particles.hpp>
#include <mos/render/decal.hpp>

namespace mos {

class RenderScene {
public:
  using Models = std::vector<mos::Model>;
  using RenderBoxes = std::vector<mos::RenderBox>;
  using Decals = std::vector<mos::Decal>;

  /**
   * @brief The Shader enum
   */
  enum class Shader { TEXT, STANDARD, EFFECT, BLUR, CRT };

  /**
   * @brief How to draw the models.
   */
  enum class Draw { TRIANGLES, LINES, POINTS };

  RenderScene();

  template <class T>
  RenderScene(T begin, T end, const RenderCamera &camera, const Light &light = Light(),
        const mos::Fog &fog_linear = Fog(),
        const Shader &shader = Shader::STANDARD,
        const Draw &draw = Draw::TRIANGLES)
      : models(begin, end), camera(camera), light(light), fog(fog_linear), shader(shader), draw(draw) {}

  RenderScene(const std::initializer_list<mos::Model> &models, const RenderCamera &camera,
        const mos::Light &light = Light(), const mos::Fog &fog = Fog(),
        const Shader &shader = Shader::STANDARD,
        const Draw &draw = Draw::TRIANGLES);
  Models models;
  Decals decals;
  Particles particles;
  RenderBoxes render_boxes;
  Light light;
  RenderCamera camera;
  Fog fog;
  Shader shader;
  Draw draw;
  void specular_environment_map(const mos::SharedTextureCube & texture);
};
}

#endif // MOS_RENDER_SCENE_HPP
