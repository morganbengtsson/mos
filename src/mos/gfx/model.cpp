#include <filesystem/path.h>
#include <iostream>
#include <mos/gfx/model.hpp>
#include <mos/util.hpp>
#include <mos/gfx/material.hpp>
#include <glm/gtx/io.hpp>

namespace mos {
namespace gfx {

Model::Model(const std::string &name, const SharedMesh &mesh,
             const glm::mat4 &transform, const Material &material)
    : mesh(mesh), material(material), name_(name), transform(transform) {}

Model::Model(Assets &assets, const nlohmann::json &json, const glm::mat4 &parent_transform) {
  auto parsed = json;
  if (parsed.is_string()) {
    std::cout << "Loading: " << parsed << std::endl;
    parsed = nlohmann::json::parse(mos::text(assets.directory() + std::string(parsed)));
  }
  auto name = parsed.value("name", "");
  auto mesh_path = std::string("");
  if (!parsed["mesh"].is_null()) {
    mesh_path = parsed.value("mesh", "");
  }
  std::string material_path;
  if (!parsed["material"].is_null()) {
    material_path = parsed.value("material", "");
  }

  name_ = name;
  mesh = assets.mesh(mesh_path);
  transform = parent_transform * jsonarray_to_mat4(parsed["transform"]);
  material = Material(assets, material_path);

  for (auto &m : parsed["children"]) {
    const std::string str_path = m;
    filesystem::path path = str_path;
    if (path.extension() == "model") {
      models.push_back(Model(assets, path.str()));
    }
  }
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
