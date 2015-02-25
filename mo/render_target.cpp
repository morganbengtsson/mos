#include "render_target.hpp"

namespace mo{

unsigned int RenderTarget::current_id_ = 0;

RenderTarget::RenderTarget(const unsigned int width, const unsigned int height)
    :texture(std::shared_ptr<Texture2D>(new Texture2D(width, height, true)))
{
    id_ = current_id_++;
}
unsigned int RenderTarget::id() const {
    return id_;
}

}
