#include <filesystem/path.h>
#include <iostream>
#include <mos/gfx/model.hpp>
#include <mos/util.hpp>
#include <mos/gfx/material.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace mos {
namespace gfx {

Model::Model(std::string name,
             Shared_mesh mesh,
             const glm::mat4 transform,
             Material material)
    : mesh(std::move(mesh)),
      material(std::move(material)),
      transform(transform),
      name_(std::move(name)) {}

Model::Model(Assets &assets, const nlohmann::json &json, const glm::mat4 &parent_transform) {
  auto parsed = json;
  if (parsed.is_string()) {
    std::cout << "Loading: " << parsed << std::endl;
	std::string path = parsed;
    parsed = nlohmann::json::parse(mos::text(assets.directory() + path));
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
glm::vec3 Model::centroid() const {
  if (mesh){
    return (transform * glm::translate(glm::mat4(1.0f), mesh->centroid))[3];
  }
  return transform[3];
}
float Model::radius() const {
  if (mesh){
    return mesh->radius;
  }
  return 0.0f;
}

}
}
