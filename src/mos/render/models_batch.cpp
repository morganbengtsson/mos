#include <mos/render/models_batch.hpp>
namespace mos {
ModelsBatch::ModelsBatch() {}

ModelsBatch::ModelsBatch(const std::initializer_list<Model> &models, const RenderCamera &camera,
             const Light &light, const FogExp &fog_exp, const FogLinear &fog_linear, const Shader &shader,
             const Draw &draw)
    : ModelsBatch(models.begin(), models.end(), camera, light,
            fog_exp, fog_linear, shader, draw) {}
}
