#include <mos/gfx/scene.hpp>
namespace mos::gfx {

Scene::Scene(Models models,
             const Camera &camera,
             const Spot_lights &lights,
             const Fog &fog,
             const Environment_lights &environment_lights,
             Boxes boxes)
    : models(std::move(models)),
      boxes(std::move(boxes)),
      spot_lights(lights),
      camera(camera),
      fog(fog),
      environment_lights(environment_lights) {}
}
