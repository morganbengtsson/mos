#include <mos/render/quad.hpp>

namespace mos {

Quad::Quad(const std::string &name, const std::shared_ptr<Mesh> &mesh,
           const std::shared_ptr<Texture> &texture)
    : Model(name, mesh, glm::mat4(1.0f), std::make_shared<Material>(texture, nullptr,
                                                                    nullptr, glm::vec3(.0f), glm::vec3(.0f), glm::vec3(.0f), 0.0f)) {
  lit = false;
}

Quad::~Quad() {}
}
