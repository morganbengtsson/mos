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

Model::Models::const_iterator Model::begin() const { return models.begin(); }

Model::Models::const_iterator Model::end() const { return models.end(); }

Model::Models::iterator Model::begin() { return models.begin(); }

Model::Models::iterator Model::end() { return models.end(); }

glm::vec4 Model::color() const {
  return glm::vec4(material.albedo, material.opacity);
}

glm::vec3 Model::position() const {
  return glm::vec3(transform[3][0], transform[3][1], transform[3][2]);
}

void Model::position(const glm::vec3 &position) {
  transform[3][0] = position[0];
  transform[3][1] = position[1];
  transform[3][2] = position[2];
}
}
}
