#include <mos/gfx/scene.hpp>
namespace mos {
namespace gfx {
Scene::Scene(){}

Scene::Scene(const Models &models,
             const Camera &camera,
             const Light &light,
             const Fog &fog,
             const OptionalEnvironmentLight &environment_light)
    : models(models),
      camera(camera),
      light(light),
      environment(environment_light),
      fog(fog){}

}
}
