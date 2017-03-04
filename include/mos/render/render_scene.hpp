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
#include <mos/render/render_cube_camera.hpp>
#include <mos/render/environment_light.hpp>

namespace mos {

class RenderScene {
public:
  using Models = std::vector<mos::Model>;
  using RenderBoxes = std::vector<mos::RenderBox>;
  using DiffuseDecals = std::vector<mos::Decal>;
  using NormalDecals = std::vector<mos::Decal>;

  /**
   * @brief The Shader enum
   */
  enum class Shader { TEXT, STANDARD, EFFECT, BLUR, CRT };

  /**
   * @brief How to draw the models.
   */
  enum class Draw { TRIANGLES, LINES, POINTS };

  RenderScene();

  template <class T, class Tc>
  RenderScene(T begin, T end, Tc cameras_begin, Tc cameras_end, const Light &light = Light(),
        const mos::Fog &fog_linear = Fog(),
        const Shader &shader = Shader::STANDARD,
        const Draw &draw = Draw::TRIANGLES)
      : models(begin, end), cameras(cameras_begin, cameras_end), light(light), fog(fog_linear), shader(shader), draw(draw) {}

  template <class T>
  RenderScene(T begin, T end, const std::initializer_list<RenderCamera> &cameras, const Light &light = Light(),
              const mos::Fog &fog_linear = Fog(),
              const Shader &shader = Shader::STANDARD,
              const Draw &draw = Draw::TRIANGLES)
      : models(begin, end), cameras(cameras.begin(), cameras.end()), light(light), fog(fog_linear), shader(shader), draw(draw) {}

  RenderScene(const std::initializer_list<Model> &models,
              const std::initializer_list<RenderCamera> &cameras,
        const mos::Light &light = Light(), const mos::Fog &fog = Fog(),
        const Shader &shader = Shader::STANDARD,
        const Draw &draw = Draw::TRIANGLES);
  Models models;
  DiffuseDecals diffuse_decals;
  NormalDecals normal_decals;
  Particles particles;
  RenderBoxes render_boxes;
  Light light;
  std::vector<RenderCamera> cameras;
  Fog fog;
  Shader shader;
  Draw draw;
  EnvironmentLight environment;
};
}

#endif // MOS_RENDER_SCENE_HPP
