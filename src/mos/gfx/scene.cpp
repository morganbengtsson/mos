#include <mos/gfx/scene.hpp>
namespace mos::gfx {

Scene::Scene(const gpu::Models &models,
             const Camera &camera,
             const Spot_lights &lights,
             const Fog &fog,
             const Environment_lights &environment_lights)
    : models(std::move(models)),
      spot_lights(lights),
      camera(camera),
      fog(fog),
      environment_lights(environment_lights) {}
}
