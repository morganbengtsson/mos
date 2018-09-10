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
      camera(camera),
      lights(lights),
      environment_lights(environment_lights),
      fog(fog),
      boxes(boxes){}
}
}
