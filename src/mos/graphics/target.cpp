#include <mos/graphics/target.hpp>

namespace mos {

std::atomic_uint Target::current_id_(0);

Target::Target(const unsigned int width, const unsigned int height)
    : texture(std::make_shared<Texture>(Texture(width, height, true))) {
  id_ = current_id_++;
}

Target::Target(const glm::ivec2 &resolution) :Target(resolution.x, resolution.y){
}

unsigned int Target::id() const { return id_; }
}
