#include <mos/graphics/batch.hpp>
namespace mos {
Batch::Batch() {}

Batch::Batch(const std::initializer_list<Model> &models, const glm::mat4 &view,
             const glm::mat4 &projection, const glm::vec2 &resolution,
             const Light &light, const FogExp &fog_exp, const FogLinear &fog_linear, const Shader &shader,
             const Draw &draw)
    : Batch(models.begin(), models.end(), view, projection, resolution, light,
            fog_exp, fog_linear, shader, draw) {}
}
