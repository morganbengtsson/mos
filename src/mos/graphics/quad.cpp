#include "graphics/quad.hpp"

namespace mos{

Quad::Quad(const std::shared_ptr<Mesh> mesh,
           const std::shared_ptr<Texture2D> texture) : Model(mesh, texture) {
    receives_light = false;
}

Quad::~Quad() {
}

}
