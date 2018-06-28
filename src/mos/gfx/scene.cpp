#include <mos/gfx/scene.hpp>
namespace mos {
namespace gfx {
Scene::Scene(){}

Scene::Scene(const Models &models,
             const Camera &camera,
             const Lights &lights,
             const Fog &fog,
             const OptionalEnvironmentLight &environment_light)
    : models(models),
      camera(camera),
      lights(lights),
      environment(environment_light),
      fog(fog){}

}
}
