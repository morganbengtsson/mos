#include <mos/gfx/scene.hpp>
namespace mos {
namespace gfx {
Scene::Scene(){}

Scene::Scene(const Models &models,
             const Camera &camera,
             const Lights &lights,
             const Fog &fog,
             const EnvironmentLights &environment_lights)
    : models(models),
      camera(camera),
      lights(lights),
      environment_lights(environment_lights),
      fog(fog){}

}
}
