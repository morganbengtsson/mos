#include <mos/render/render_assets.hpp>
#include <cstring>
#include <filesystem/path.h>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/io.hpp>
#include <mos/util.hpp>

namespace mos {
using namespace std;
using namespace glm;
using namespace nlohmann;

RenderAssets::RenderAssets(const std::string directory) : directory_(directory) {}

RenderAssets::~RenderAssets() {}

Model RenderAssets::model_value(const std::string & base_path, const json &value) {
  auto name = value.value("name", "");
  auto mesh_name = std::string("");
  if (!value["mesh"].is_null()) {
    mesh_name = value.value("mesh", "");
  }
  std::string material_name = "";
  if (!value["material"].is_null()){
    material_name = value.value("material", "");
  }

  auto transform = jsonarray_to_mat4(value["transform"]);

  auto created_model = mos::Model(
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

Animation RenderAssets::animation(const string &path) {
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
                       const bool compress,
                       const Texture2D::Wrap &wrap) {
  if (!path.empty()) {
    if (textures_.find(path) == textures_.end()) {
      textures_.insert(TexturePair(path, Texture2D::load(directory_ + path, mipmaps, compress, wrap)));
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
        std::string t = "";
        if (!value["diffuse_map"].is_null()) {
          t = value["diffuse_map"];
        }
        auto diffuse_map = t.empty() ? texture("") : texture(base_path + t);

        std::string e = "";
        if (!value["emission_map"].is_null()) {
          e = value["emission_map"];
        }
        auto emission_map = e.empty() ? texture("") : texture(base_path + e);

        std::string n = "";
        if (!value["normal_map"].is_null()) {
          n = value["normal_map"];
        }
        auto normal_map = n.empty() ? texture("") : texture(base_path + n);

        std::string l = "";
        if (!value["light_map"].is_null()) {
          l = value["light_map"];
        }
        auto light_map = l.empty() ? texture("") : texture(base_path + l);

        auto ambient = glm::vec3(value["ambient"][0], value["ambient"][1], value["ambient"][2]);
        auto diffuse = glm::vec3(value["diffuse"][0], value["diffuse"][1], value["diffuse"][2]);
        auto specular = glm::vec3(value["specular"][0], value["specular"][1], value["specular"][2]);
        auto emissive = glm::vec3(value["emission"][0], value["emission"][1], value["emission"][2]);
        auto opacity = value["opacity"];
        auto shininess = value["shininess"];
        return Material(diffuse_map, emission_map, normal_map, light_map, ambient, diffuse, specular, emissive, opacity, shininess);
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
