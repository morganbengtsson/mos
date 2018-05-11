#include <mos/gfx/assets.hpp>
#include <cstring>
#include <filesystem/path.h>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <mos/util.hpp>
#include <iostream>

namespace mos {
namespace gfx {
using namespace nlohmann;

Assets::Assets(const std::string directory) : directory_(directory) {}

Assets::~Assets() {}

Model Assets::model_value(const std::string &base_path, const json &value) {
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

  for (auto &m : value["children"]) {
    created_model.models.push_back(model(base_path + std::string(m)));
  }
  return created_model;
}

Model Assets::model(const std::string &path) {
  std::cout << "Loading : " << path << std::endl;
  filesystem::path fpath = path;
  auto doc = json::parse(mos::text(directory_ + path));

  return model_value(fpath.parent_path().empty() ? "" : fpath.parent_path().str() + "/", doc);
}

Animation Assets::animation(const std::string &path) {
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

std::shared_ptr<Mesh> Assets::mesh(const std::string &path) {
  if (meshes_.find(path) == meshes_.end()) {
    meshes_.insert(MeshPair(path, Mesh::load(directory_ + path)));
  }
  return meshes_.at(path);
}

std::shared_ptr<Texture2D>
Assets::texture(const std::string &path,
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

Material Assets::material(const std::string &path) {
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

      auto albedo_map = read_texture("albedo_map");
      auto emission_map = read_texture("emission_map");
      auto normal_map = read_texture("normal_map");
      if (normal_map) {
        if (normal_map->format == Texture::Format::SRGB){
          normal_map->format = Texture::Format::RGB;
        }
        else if (normal_map->format == Texture::Format::SRGBA){
          normal_map->format = Texture::Format::RGBA;
        }
        //normal_map->format = normal_map->format == Texture::Format::SRGB ? Texture::Format::RGB : Texture::Format::RGBA;
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
      return Material(albedo_map,
                      emission_map,
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

Light Assets::light(const std::string &path) {
  if (path.empty()) {
    return Light();
  } else {
    filesystem::path fpath = path;
    auto base_path = fpath.parent_path().empty() ? "" : fpath.parent_path().str() + "/";

    if (fpath.extension() == "light") {
      auto value = json::parse(mos::text(directory_ + fpath.str()));

      auto transform = jsonarray_to_mat4(value["transform"]);
      auto position = glm::vec3(transform[3]);
      auto center = position + glm::vec3(transform * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));

      auto data_value = json::parse(mos::text(directory_ + std::string(value["light"])));

      auto color = glm::vec3(data_value["color"][0],
                             data_value["color"][1],
                             data_value["color"][2]);
      auto strength = data_value["strength"];
      auto size = data_value["size"];
      auto blend = value["blend"];

      return Light(position,
                      center,
                      size,
                      color,
                      strength);
    } else {
      throw std::runtime_error(path.substr(path.find_last_of(".")) +
          " file format is not supported.");
    }
  }
}

EnvironmentLight Assets::environment_light(const std::string &path) {
  filesystem::path fpath = path;
  auto base_path = fpath.parent_path().empty() ? "" : fpath.parent_path().str() + "/";

  if (fpath.extension() == "environment_light") {
    auto value = json::parse(mos::text(directory_ + fpath.str()));

    auto transform = jsonarray_to_mat4(value["transform"]);

    auto position = glm::vec3(transform[3]);

    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(transform, scale, rotation, translation, skew, perspective);

    auto extent = float(value["extent"]) * scale;
    auto strength = value.value("strength", 1.0f);
    auto resolution = value.value("resolution", 128.0f);

    return EnvironmentLight(position,
                 extent,
                 strength,
                 glm::uvec2(resolution));
  } else {
    throw std::runtime_error(path.substr(path.find_last_of(".")) +
        " file format is not supported.");
  }
}

void Assets::clear_unused() {
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
