#include <mos/render/render_scene.hpp>
namespace mos {
RenderScene::RenderScene() {}

RenderScene::RenderScene(const std::initializer_list<Model> &models, const RenderCamera &camera,
             const Light &light, const Fog &fog, const Shader &shader,
             const Draw &draw)
    : RenderScene(models.begin(), models.end(), camera, light,
            fog, shader, draw) {}

void RenderScene::specular_environment_map(const mos::SharedTextureCube &texture) {
  // TODO: model childrens
  for (auto & model : models) {
    model.material->environment_map = texture;
  }

}
}
