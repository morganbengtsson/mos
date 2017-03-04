#include <filesystem/path.h>
#include <mos/render/model.hpp>
#include <mos/util.hpp>

namespace mos {

Model::Model() {}

/*
Model::Model(const mos::Model &model, const glm::mat4 transform) : Model(model),
transform(transform) {
}
 */

Model::Model(const std::string &path)
    : Model(path, json::parse(mos::text(path))) {}

Model::Model(const std::string &path, const json &value) {
  filesystem::path fpath = path;

  auto name = value.value("name", "");
  auto mesh_name = value.value("mesh", "");
  auto texture_name = value.value("texture", "");
  auto lightmap_name =
      value["lightmap"].is_null() ? "" : value.value("lightmap", "");
  auto normalmap_name = value.value("normalmap", "");
  std::string material_name = value.value("material", "");

  auto t = jsonarray_to_mat4(value["transform"]);

  name_ = name, mesh = Mesh::load(fpath.parent_path().str() + "/" + mesh_name);
  //  texture = Texture::load(directory + "/" + texture_name);
  transform = t;
  material = Material::load(fpath.parent_path().str() + "/" + material_name);
  // normalmap = Texture::load(directory + "/" + normalmap_name);

  for (auto &v : value["models"]) {
    models.push_back(Model(fpath.parent_path().str(), v));
  }

  // TODO Put in to other constructor?
}

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
}
