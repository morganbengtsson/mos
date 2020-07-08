#pragma once
#include <mos/gfx/model.hpp>
#include <mos/gfx/camera.hpp>
#include <mos/gfx/light.hpp>
#include <mos/gfx/fog.hpp>
#include <mos/gfx/target.hpp>
#include <mos/gfx/boxes.hpp>
#include <mos/gfx/clouds.hpp>
#include <mos/gfx/cube_camera.hpp>
#include <mos/gfx/environment_lights.hpp>
#include <mos/gfx/texture_targets.hpp>
#include <mos/gfx/models.hpp>
#include <mos/gfx/lights.hpp>
#include <mos/gfx/directional_light.hpp>
#include <mos/core/container.hpp>

namespace mos::gfx {

/** Scene for rendering. */
class Scene {
public:
  Scene() = default;
  Scene(Models models,
        const Camera &camera,
        const Lights &lights = Lights(),
        const Fog &fog = Fog(),
        const Environment_lights &environment_lights = Environment_lights(),
        Boxes boxes = Boxes());
  Model sky;
  Models models;
  Clouds point_clouds;
  Clouds line_clouds;
  Boxes boxes;
  Lights lights;
  Directional_light directional_light;
  Camera camera;
  Fog fog;
  Environment_lights environment_lights;
  Texture_targets texture_targets;
};
}

