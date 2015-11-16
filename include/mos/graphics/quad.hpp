#ifndef MOS_QUAD_HPP
#define MOS_QUAD_HPP

#include "model.hpp"
#include "texture2d.hpp"
#include "mesh.hpp"

#include <memory>

namespace mos {

class Quad : public mos::Model
{
public:
    Quad(const std::string & name,
         const std::shared_ptr<Mesh> mesh,
         const std::shared_ptr<Texture2D> texture);
    virtual ~Quad();
};

}

#endif // MOS_QUAD_HPP
