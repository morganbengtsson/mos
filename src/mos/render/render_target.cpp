#include <mos/render/render_target.hpp>

namespace mos {

std::atomic_uint RenderTarget::current_id_(0);

RenderTarget::RenderTarget(const unsigned int width, const unsigned int height)
    : texture(std::make_shared<Texture>(Texture(width, height, true))), id_(current_id_++) {}

RenderTarget::RenderTarget(const glm::ivec2 &resolution) : RenderTarget(resolution.x, resolution.y){
}

unsigned int RenderTarget::id() const { return id_; }
}
