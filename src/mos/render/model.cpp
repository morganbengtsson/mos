#include <mos/render/model.hpp>
#include <mos/util.hpp>
#include <filesystem/path.h>

namespace mos {

Model::Model() {}

/*
Model::Model(const mos::Model &model, const glm::mat4 transform) : Model(model), transform(transform) {
}
 */

Model::Model(const std::string &path)
    : Model(path, json::parse(mos::text(path))) {}

Model::Model(const std::string &path, const json &value) : {
  filesystem::path fpath = path;

  auto name = value.value("name", "");
  auto mesh_name = value.value("mesh", "");
  auto texture_name = value.value("texture", "");
  auto lightmap_name =
      value["lightmap"].is_null() ? "" : value.value("lightmap", "");
  auto normalmap_name = value.value("normalmap", "");
  std::string material_name = value.value("material", "");
  bool recieves_light = value.value("receives_light", true);

  auto t = jsonarray_to_mat4(value["transform"]);

  name_ = name, mesh = Mesh::load(fpath.parent_path().str() + "/" + mesh_name);
  //  texture = Texture::load(directory + "/" + texture_name);
  transform = t;
  material = Material::load(fpath.parent_path().str() + "/" + material_name);
  //normalmap = Texture::load(directory + "/" + normalmap_name);
  lit = recieves_light;

  for (auto &v : value["models"]) {
    models.push_back(Model(fpath.parent_path().str(), v));
  }

  //TODO Put in to other constructor?
}

Model::Model(const std::string &name, const SharedMesh &mesh,
             const glm::mat4 &transform,
             const Material &material,
               const bool lit)
    : mesh(mesh),  material(material),
      name_(name), transform(transform),
      lit(lit), overlay_(0.0f), multiply_(1.0f) {}

Model::~Model() {}

std::string Model::name() const { return name_; }

Model::Models::const_iterator Model::begin() const { return models.begin(); }

Model::Models::const_iterator Model::end() const { return models.end(); }

Model::Models::iterator Model::begin() { return models.begin(); }

Model::Models::iterator Model::end() { return models.end(); }

glm::vec4 Model::color() const {
  return glm::vec4(material.diffuse, material.opacity);
}

glm::vec3 Model::position() const {
  return glm::vec3(transform[3][0], transform[3][1], transform[3][2]);
}

void Model::position(const glm::vec3 &position) {
  transform[3][0] = position[0];
  transform[3][1] = position[1];
  transform[3][2] = position[2];
}

glm::vec4 Model::overlay() const { return overlay_; }

void Model::overlay(const glm::vec4 &overlay) {
  overlay_ = overlay;
  for (auto &model : models) {
    model.overlay(overlay);
  }
}

void Model::overlay(const glm::vec3 &color, float alpha) {
  overlay(glm::vec4(color, alpha));
}

glm::vec3 Model::multiply() const { return multiply_; }

void Model::multiply(const glm::vec3 &multiply) {
  multiply_ = multiply;
  for (auto &model : models) {
    model.multiply(multiply);
  }
}

}
