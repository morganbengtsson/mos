#include <mos/render/render_scene.hpp>
namespace mos {
namespace gfx {
RenderScene::RenderScene() {}

RenderScene::RenderScene(const std::initializer_list<Model> &models,
                         const RenderCamera &camera,
                         const Light &light,
                         const EnvironmentLight &environment_light,
                         const Fog &fog,
                         const Shader &shader,
                         const Draw &draw)
    : RenderScene(models.begin(),
                  models.end(),
                  camera,
                  light,
                  environment_light,
                  fog,
                  shader,
                  draw) {}

}
}
