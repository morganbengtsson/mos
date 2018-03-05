#pragma once

#include <vector>
#include <initializer_list>
#include <mos/render/model.hpp>
#include <mos/render/camera.hpp>
#include <mos/render/light.hpp>
#include <mos/render/fog.hpp>
#include <mos/render/target.hpp>
#include <mos/render/texture_cube.hpp>
#include <mos/render/box.hpp>
#include <mos/render/particles.hpp>
#include <mos/render/decal.hpp>
#include <mos/render/cube_camera.hpp>
#include <mos/render/environment_light.hpp>

namespace mos {
namespace gfx {

class Scene {
public:
  using Models = std::vector<Model>;
  using RenderBoxes = std::vector<Box>;
  using Decals = std::array<Decal, 10>;
  using ParticleClouds = std::vector<Particles>;

  /**
   * @brief The Shader enum
   */
  enum class Shader { TEXT, STANDARD, EFFECT, BLUR, CRT, DEPTH };

  /**
   * @brief How to draw the models.
   */
  enum class Draw { TRIANGLES, LINES, POINTS };

  Scene();

  template<class T>
  Scene(T begin,
              T end,
              const Camera &camera,
              const Light &light = Light(),
              const EnvironmentLight &environment_light = EnvironmentLight(),
              const Fog &fog_linear = Fog(),
              const Shader &shader = Shader::STANDARD,
              const Draw &draw = Draw::TRIANGLES)
      : models(begin, end),
        camera(camera),
        light(light),
        environment(environment_light),
        fog(fog_linear),
        shader(shader),
        draw(draw) {}

  Scene(const std::initializer_list<Model> &models,
              const Camera &camera,
              const Light &light = Light(),
              const EnvironmentLight &environment_light = EnvironmentLight(),
              const Fog &fog = Fog(),
              const Shader &shader = Shader::STANDARD,
              const Draw &draw = Draw::TRIANGLES);
  Models models;
  Decals decals;
  ParticleClouds particle_clouds;
  RenderBoxes render_boxes;
  Light light;
  Camera camera;
  Fog fog;
  Shader shader;
  Draw draw;
  EnvironmentLight environment;
};
}
}