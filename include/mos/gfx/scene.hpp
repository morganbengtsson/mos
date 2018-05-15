#pragma once
#include <vector>
#include <initializer_list>
#include <mos/gfx/model.hpp>
#include <mos/gfx/camera.hpp>
#include <mos/gfx/light.hpp>
#include <mos/gfx/fog.hpp>
#include <mos/gfx/target.hpp>
#include <mos/gfx/texture_cube.hpp>
#include <mos/gfx/box.hpp>
#include <mos/gfx/particle_cloud.hpp>
#include <mos/gfx/cube_camera.hpp>
#include <mos/gfx/environment_light.hpp>
#include <mos/gfx/texture_target.hpp>
#include <mos/gfx/models.hpp>

namespace mos {
namespace gfx {

/** Scene for rendering. */
class Scene {
public:
  using TextureTargets = std::vector<TextureTarget>;
  using Boxes = std::vector<Box>;
  using ParticleClouds = std::vector<ParticleCloud>;

  Scene();

  template<class T>
  Scene(T begin,
        T end,
        const Camera &camera,
        const Light &light = Light(),
        const EnvironmentLight &environment_light = EnvironmentLight(),
        const Fog &fog_linear = Fog())
      : models(begin, end),
        camera(camera),
        light(light),
        environment(environment_light),
        fog(fog_linear){}

  Scene(const std::initializer_list<Model> &models,
        const Camera &camera,
        const Light &light = Light(),
        const EnvironmentLight &environment_light = EnvironmentLight(),
        const Fog &fog = Fog());
  Models models;
  ParticleClouds particle_clouds;
  Boxes boxes;
  Light light;
  Camera camera;
  Fog fog;
  EnvironmentLight environment;
  TextureTargets texture_targets;
};
}
}