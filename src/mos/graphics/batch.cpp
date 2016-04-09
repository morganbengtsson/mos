#include <mos/graphics/batch.hpp>
namespace mos {
Batch::Batch() {}

Batch::Batch(const std::initializer_list<Model> &models, const glm::mat4 &view,
             const glm::mat4 &projection, const glm::vec2 &resolution,
             const Light &light, const Fog &fog, const Shader &shader,
             const Draw &draw)
    : Batch(models.begin(), models.end(), view, projection, resolution, light,
            fog, shader, draw) {}
}
