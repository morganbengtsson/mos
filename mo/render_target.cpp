#include "render_target.hpp"

namespace mo{

RenderTarget::RenderTarget()
    :texture(std::shared_ptr<Texture2D>(new Texture2D(1024, 768, true)))
{
    static unsigned int current_id = 0;
    id_ = current_id++;
}
unsigned int RenderTarget::id() const {
    return id_;
}

}
