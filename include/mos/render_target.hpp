#ifndef RENDER_TARGET_HPP
#define RENDER_TARGET_HPP

#include <memory>
#include "texture2d.hpp"

namespace mos {
class RenderTarget {
public:
    explicit RenderTarget(const unsigned int width = 256,  const unsigned int height = 256);
    std::shared_ptr<mos::Texture2D> texture;
    
	/*!
     * \brief unique id
     * \return id
     */
    unsigned int id() const;
private:
    unsigned int id_;
    static unsigned int current_id_;
};

}
#endif // RENDER_TARGET_HPP
