#include <filesystem/path.h>
#include <mos/gfx/model.hpp>
#include <mos/util.hpp>

namespace mos {
namespace gfx {

Model::Model() {}

Model::Model(const std::string &name, const SharedMesh &mesh,
             const glm::mat4 &transform, const Material &material)
    : mesh(mesh), material(material), name_(name), transform(transform) {}

Model::~Model() {}

std::string Model::name() const { return name_; }

glm::vec3 Model::position() const {
  return glm::vec3(transform[3]);
}

void Model::position(const glm::vec3 &position) {
  transform[3][0] = position[0];
  transform[3][1] = position[1];
  transform[3][2] = position[2];
}
}
}
