#pragma once
#include <mos/gfx/model.hpp>
#include <mos/gfx/camera.hpp>
#include <mos/gfx/spot_light.hpp>
#include <mos/gfx/fog.hpp>
#include <mos/gfx/target.hpp>
#include <mos/gfx/boxes.hpp>
#include <mos/gfx/clouds.hpp>
#include <mos/gfx/cube_camera.hpp>
#include <mos/gfx/environment_lights.hpp>
#include <mos/gfx/texture_targets.hpp>
#include <mos/gfx/models.hpp>
#include <mos/gfx/spot_lights.hpp>
#include <mos/gfx/directional_light.hpp>
#include <mos/gfx/renderer.hpp>
#include <mos/core/container.hpp>

namespace mos::gfx {

/** Scene for rendering. */
class Scene {
public:
  Scene() = default;
  Scene(const gpu::Models &models,
        const Camera &camera,
        const Spot_lights &spot_lights = Spot_lights(),
        const Fog &fog = Fog(),
        const Environment_lights &environment_lights = Environment_lights(),
        Boxes boxes = Boxes());
  gpu::Model sky = gpu::Model();
  gpu::Models models;
  Clouds point_clouds;
  Clouds line_clouds;
  Boxes boxes;
  Spot_lights spot_lights;
  Directional_light directional_light;
  Camera camera;
  Fog fog;
  Environment_lights environment_lights;
  Texture_targets texture_targets;
};
}

