#include <mos/graphics/quad.hpp>

namespace mos {

Quad::Quad(const std::string &name, const std::shared_ptr<Mesh> &mesh,
           const std::shared_ptr<Texture> &texture)
    : Model(name, mesh, texture) {
  receives_light = false;
}

Quad::~Quad() {}
}
