#include <mos/render/render_assets.hpp>
#include <cstring>
#include <filesystem/path.h>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/io.hpp>
#include <mos/util.hpp>
#include <iostream>

namespace mos {
namespace gfx {
using namespace nlohmann;

RenderAssets::RenderAssets(const std::string directory) : directory_(directory) {}

RenderAssets::~RenderAssets() {}

Model RenderAssets::model_value(const std::string &base_path, const json &value) {
  auto name = value.value("name", "");
  auto mesh_name = std::string("");
  if (!value["mesh"].is_null()) {
    mesh_name = value.value("mesh", "");
  }
  std::string material_name = "";
  if (!value["material"].is_null()) {
    material_name = value.value("material", "");
  }

  auto transform = jsonarray_to_mat4(value["transform"]);

  auto created_model = Model(
      name, mesh(base_path + mesh_name),
      transform,
      material(base_path + material_name));

  for (auto &m : value["models"]) {
    created_model.models.push_back(model_value(base_path, m));
  }
  return created_model;
}

Model RenderAssets::model(const std::string &path) {
  std::cout << "Loading : " << path << std::endl;
  filesystem::path fpath = path;
  auto doc = json::parse(mos::text(directory_ + path));

  return model_value(fpath.parent_path().empty() ? "" : fpath.parent_path().str() + "/", doc);
}

Animation RenderAssets::animation(const std::string &path) {
  auto doc = json::parse(mos::text(directory_ + path));
  auto frame_rate = doc["frame_rate"];
  std::map<unsigned int, std::shared_ptr<Mesh const>> keyframes;

  for (auto &keyframe : doc["keyframes"]) {
    auto key = keyframe["key"];
    auto mesh_path = keyframe["mesh"];
    keyframes.insert({key, mesh(mesh_path)});
  }
  Animation animation(keyframes, frame_rate);
  return animation;
}

std::shared_ptr<Mesh> RenderAssets::mesh(const std::string &path) {
  if (meshes_.find(path) == meshes_.end()) {
    meshes_.insert(MeshPair(path, Mesh::load(directory_ + path)));
  }
  return meshes_.at(path);
}

std::shared_ptr<Texture2D>
RenderAssets::texture(const std::string &path,
                      const bool mipmaps,
                      const Texture2D::Wrap &wrap) {
  if (!path.empty()) {
    if (textures_.find(path) == textures_.end()) {
      textures_.insert(TexturePair(path, Texture2D::load(directory_ + path, mipmaps, wrap)));
    }
    return textures_.at(path);
  } else {
    return std::shared_ptr<Texture2D>(nullptr);
  }
}

Material RenderAssets::material(const std::string &path) {
  if (path.empty()) {
    return Material();
  } else {
    filesystem::path fpath = path;
    auto base_path = fpath.parent_path().empty() ? "" : fpath.parent_path().str() + "/";

    if (fpath.extension() == "material") {
      auto value = json::parse(mos::text(directory_ + fpath.str()));

      auto read_texture = [&](const std::string &name) {
        std::string file_name = "";
        if (!value[name].is_null()) {
          file_name = value[name];
        }
        auto tex = file_name.empty() ? texture("") : texture(base_path + file_name);
        return tex;
      };

      auto diffuse_map = read_texture("albedo_map");
      auto normal_map = read_texture("normal_map");
      if (normal_map) {
        normal_map->format = Texture::Format::RGBA;
      }
      auto metallic_map = read_texture("metallic_map");
      auto roughness_map = read_texture("roughness_map");
      auto ambient_occlusion_map = read_texture("ambient_occlusion_map");

      auto diffuse = glm::vec3(value["albedo"][0], value["albedo"][1], value["albedo"][2]);
      auto opacity = value["opacity"];
      auto roughness = value["roughness"];
      auto metallic = value["metallic"];
      auto emission = glm::vec3(value["emission"][0], value["emission"][1], value["emission"][2]);
      auto ambient_occlusion = value["ambient_occlusion"];
      return Material(diffuse_map,
                      normal_map,
                      metallic_map,
                      roughness_map,
                      ambient_occlusion_map,
                      diffuse,
                      opacity,
                      roughness,
                      metallic,
                      emission);
    } else {
      throw std::runtime_error(path.substr(path.find_last_of(".")) +
          " file format is not supported.");
    }
  }
}

void RenderAssets::clear_unused() {
  for (auto it = textures_.begin(); it != textures_.end();) {
    if (it->second.use_count() <= 1) {
      textures_.erase(it++);
    } else {
      ++it;
    }
  }
  for (auto it = meshes_.begin(); it != meshes_.end();) {
    if (it->second.use_count() <= 1) {
      meshes_.erase(it++);
    } else {
      ++it;
    }
  }
}
}
}
