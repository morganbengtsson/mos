#include <mos/render/render_target_cube.hpp>

namespace mos {

std::atomic_uint RenderTargetCube::current_id_(0);

RenderTargetCube::RenderTargetCube(const unsigned int width, const unsigned int height)
    : texture(std::make_shared<TextureCube>(TextureCube(width, height, true))), id_(current_id_++) {}

RenderTargetCube::RenderTargetCube(const glm::ivec2 &resolution) : RenderTargetCube(resolution.x, resolution.y){
}

unsigned int RenderTargetCube::id() const { return id_; }
}
