#include "render_target.hpp"

namespace mo{

unsigned int RenderTarget::current_id_ = 0;

RenderTarget::RenderTarget(const unsigned int width, const unsigned int height)
    :texture(std::make_shared<Texture2D>(Texture2D(width, height, true))) {
    id_ = current_id_++;
}
unsigned int RenderTarget::id() const {
    return id_;
}

}
