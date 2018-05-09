#include <mos/gfx/scene.hpp>
namespace mos {
namespace gfx {
Scene::Scene() {}

Scene::Scene(const std::initializer_list<Model> &models,
                         const Camera &camera,
                         const Light &light,
                         const EnvironmentLight &environment_light,
                         const Fog &fog)
    : Scene(models.begin(),
                  models.end(),
                  camera,
                  light,
                  environment_light,
                  fog) {}

}
}
