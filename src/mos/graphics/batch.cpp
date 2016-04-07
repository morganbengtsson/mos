#include <mos/graphics/batch.hpp>
namespace mos {
Batch::Batch() {}

Batch::Batch(const std::initializer_list<Model> &models, const glm::mat4 &view,
             const glm::mat4 &projection, const Light &light, const Fog &fog,
             const glm::vec2 &resolution)
    : Batch(models.begin(), models.end(), view, projection, light, fog,
            resolution) {}
}
