#include <mos/gfx/scene.hpp>
namespace mos {
namespace gfx {
Scene::Scene(){}

Scene::Scene(const Models &models,
             const Camera &camera,
             const Lights &lights,
             const Fog &fog,
             const Environment_lights &environment_lights,
             const Boxes &boxes)
    : models(models),      
      boxes(boxes),
      lights(lights),
      camera(camera),
      fog(fog),
      environment_lights(environment_lights) {}
}
}
