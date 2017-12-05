#include <mos/render/render_target.hpp>
#include <mos/render/texture_cube.hpp>

namespace mos {

std::atomic_uint RenderTarget::current_id_;

unsigned int RenderTarget::id() const { return id_; }

RenderTarget::RenderTarget() :
   id_(current_id_++) {
}
}
