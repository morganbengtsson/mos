#include <mos/assets.hpp>

#include <algorithm>
#include <cstring>
#include <exception>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/io.hpp>
#include <iostream>
#include <iterator>
#include <lodepng.h>
#include <memory>
#include <mos/util.hpp>
#include <stb_vorbis.h>

namespace mos {
using namespace std;
using namespace glm;
using namespace nlohmann;

Assets::Assets(const std::string directory) : directory_(directory) {}

Assets::~Assets() {}

Model Assets::model_value(const json &value) {
  auto name = value.value("name", "");
  auto mesh_name = std::string("");
  if (!value["mesh"].is_null()) {
    mesh_name = value.value("mesh", "");
  }

  std::string material_name = value.value("material", "");
  bool recieves_light = value.value("receives_light", true);
  auto transform = jsonarray_to_mat4(value["transform"]);

  auto created_model = mos::Model(
      name, mesh(mesh_name),
      transform,
      material(material_name),
      recieves_light);

  for (auto &m : value["models"]) {
    created_model.models.push_back(model_value(m));
  }
  return created_model;
}

Model Assets::model(const std::string &path) {
  std::cout << "Loading : " << path << std::endl;
  auto doc = json::parse(mos::text(directory_ + path));
  return model_value(doc);
}

Animation Assets::animation(const string &path) {
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

std::shared_ptr<Texture>
Assets::texture(const std::string &path,
                       const bool mipmaps,
                       const bool compress,
                       const Texture::Wrap &wrap) {
  if (!path.empty()) {
    if (textures_.find(path) == textures_.end()) {
      textures_.insert(TexturePair(path, Texture::load(directory_ + path, mipmaps, compress, wrap)));
    }
    return textures_.at(path);
  } else {
    return std::shared_ptr<Texture>(nullptr);
  }
}



std::shared_ptr<AudioBuffer>
Assets::audio_buffer(const std::string &path) {
  if (sounds_.find(path) == sounds_.end()) {
    sounds_.insert(AudioBufferPair(path, AudioBuffer::load(directory_ + path)));
    return sounds_.at(path);
  } else {
    return sounds_.at(path);
  }
}

std::shared_ptr<Material> Assets::material(const std::string &path) {
  if (materials_.find(path) == materials_.end()) {
    materials_.insert(MaterialPair(path, Material::load(directory_ + path)));
    return materials_.at(path);
  } else {
    return materials_.at(path);
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
  for (auto it = sounds_.begin(); it != sounds_.end();) {
    if (it->second.use_count() <= 1) {
      sounds_.erase(it++);
    } else {
      ++it;
    }
  }
  for (auto it = materials_.begin(); it != materials_.end();) {
    if (it->second.use_count() <= 1) {
      materials_.erase(it++);
    } else {
      ++it;
    }
  }
}
}
