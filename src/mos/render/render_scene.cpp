#include <mos/render/render_scene.hpp>
namespace mos {
RenderScene::RenderScene() {}

RenderScene::RenderScene(const std::initializer_list<Model> &models, const RenderCamera &camera,
             const Light &light, const Fog &fog, const Shader &shader,
             const Draw &draw)
    : RenderScene(models.begin(), models.end(), camera, light,
            fog, shader, draw) {}

}
