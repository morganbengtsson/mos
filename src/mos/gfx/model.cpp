#include <filesystem/path.h>
#include <iostream>
#include <mos/gfx/model.hpp>
#include <mos/util.hpp>

namespace mos {
namespace gfx {

Model::Model(const std::string &name, const SharedMesh &mesh,
             const glm::mat4 &transform, const Material &material)
    : mesh(mesh), material(material), name_(name), transform(transform) {}

Model::Model(Assets &assets, const nlohmann::json &doc, const glm::mat4 &parent_transform) {
  auto name = doc.value("name", "");
  auto mesh_path = std::string("");
  if (!doc["mesh"].is_null()) {
    mesh_path = doc.value("mesh", "");
  }
  std::string material_path;
  if (!doc["material"].is_null()) {
    material_path = doc.value("material", "");
  }

  name_ = name;
  mesh = assets.mesh(mesh_path);
  transform = parent_transform * jsonarray_to_mat4(doc["transform"]);
  material = assets.material(material_path);

  for (auto &m : doc["children"]) {
    const std::string str_path = m;
    filesystem::path path = str_path;
    if (path.extension() == "model") {
      models.push_back(Model(assets, path.str()));
    }
  }
}

Model::Model(Assets &assets, const std::string &path, const glm::mat4 &parent_transform) :
    Model(assets, nlohmann::json::parse(mos::text(assets.directory() + path)), parent_transform) {
  std::cout << "Loaded: " << path << std::endl;
}

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
