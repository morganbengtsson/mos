#include <mos/graphics/model.hpp>

namespace mos {

Model::Model() {}

Model::Model(const std::string &name, const std::shared_ptr<Mesh> &mesh,
             const Textures &textures,
             const glm::mat4 &transform,
             const std::shared_ptr<Material> &material,
             const Lightmaps &lightmaps,
             const std::shared_ptr<Texture> &normalmap,
             const float affected_by_light)
    : mesh(mesh), textures(textures),
      material(material), lightmaps(lightmaps), normalmap(normalmap), name_(name),
      transform(transform), receives_light(affected_by_light),
      overlay_(0.0f), multiply_(1.0f) {

}

Model::~Model() {}

std::string Model::name() const { return name_; }

Model::Models::const_iterator Model::begin() const { return models.begin(); }

Model::Models::const_iterator Model::end() const { return models.end(); }

Model::Models::iterator Model::begin() { return models.begin(); }

Model::Models::iterator Model::end() { return models.end(); }

glm::vec4 Model::color() const {
  return glm::vec4(material->diffuse, material->opacity);
}

glm::vec3 Model::position() const {
  return glm::vec3(transform[3][0], transform[3][1], transform[3][2]);
}

void Model::position(const glm::vec3 &position) {
  transform[3][0] = position[0];
  transform[3][1] = position[1];
  transform[3][2] = position[2];
}

glm::vec3 Model::overlay() const {
  return overlay_;
}

void Model::overlay(const glm::vec3 & overlay) {
  overlay_ = overlay;
  for (auto & model : models){
    model.overlay(overlay);
  }
}

float Model::multiply() const {
  return multiply_;
}

void Model::multiply(const float & multiply) {
  multiply_ = multiply;
  for (auto & model : models){
    model.multiply(multiply);
  }
}
}
