#pragma once
#include <vector>
#include <initializer_list>
#include <mos/gfx/model.hpp>
#include <mos/gfx/camera.hpp>
#include <mos/gfx/light.hpp>
#include <mos/gfx/fog.hpp>
#include <mos/gfx/target.hpp>
#include <mos/gfx/box.hpp>
#include <mos/gfx/particle_cloud.hpp>
#include <mos/gfx/cube_camera.hpp>
#include <mos/gfx/environment_light.hpp>
#include <mos/gfx/texture_target.hpp>
#include <mos/gfx/models.hpp>
#include <mos/gfx/lights.hpp>
#include <mos/core/container.hpp>

namespace mos {
namespace gfx {

using ParticleClouds = Container<ParticleCloud>;
using TextureTargets = Container<TextureTarget>;
using EnvironmentLights = std::array<EnvironmentLight, 2>;
using Boxes = Container<Box>;

/** Scene for rendering. */
class Scene {
public:
  Scene();
  Scene(const Models &models,
        const Camera &camera,
        const Lights &lights = Lights(),
        const Fog &fog = Fog(),
        const EnvironmentLights &environment_lights = EnvironmentLights(),
        const Boxes &boxes = Boxes());

  Models models;
  ParticleClouds particle_clouds;
  Boxes boxes;
  Lights lights;
  Camera camera;
  Fog fog;
  EnvironmentLights environment_lights;
  TextureTargets texture_targets;
};
}
}