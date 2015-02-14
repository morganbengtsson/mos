#ifndef RENDER_TARGET_HPP
#define RENDER_TARGET_HPP

#include <memory>
#include "texture2d.hpp"

namespace mo {
class RenderTarget {
public:
    RenderTarget();
    std::shared_ptr<mo::Texture2D> texture;
    /*!
     * \brief unique id
     * \return id
     */
    unsigned int id() const;
private:
    unsigned int id_;
};

}
#endif // RENDER_TARGET_HPP
