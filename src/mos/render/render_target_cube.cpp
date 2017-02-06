#include <mos/render/render_target_cube.hpp>

namespace mos {

std::atomic_uint RenderTargetCube::current_id_(0);

RenderTargetCube::RenderTargetCube(const float width, const float height)
    : texture(std::make_shared<TextureCube>(TextureCube(width, height, true))), id_(current_id_++), layers(6) {}

RenderTargetCube::RenderTargetCube(const glm::vec2 &resolution) : RenderTargetCube(resolution.x, resolution.y){
}

unsigned int RenderTargetCube::id() const { return id_; }
}
