#include <mos/gfx/scene.hpp>
namespace mos::gfx {

Scene::Scene(Models models,
             const Camera &camera,
             const Lights &lights,
             const Fog &fog,
             const Environment_lights &environment_lights,
             Boxes boxes)
    : models(std::move(models)),
      boxes(std::move(boxes)),
      lights(lights),
      camera(camera),
      fog(fog),
      environment_lights(environment_lights) {}
}
