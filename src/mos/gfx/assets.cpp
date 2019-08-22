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

Assets::Assets(std::string directory) : directory_(std::move(directory)) {}

std::shared_ptr<Mesh> Assets::mesh(const std::string &path) {
  if (path.empty()){
    return Shared_mesh(nullptr);
  }
  if (meshes_.find(path) == meshes_.end()) {
    meshes_.insert(Mesh_pair(path, Mesh::load(directory_ + path)));
  }
  return meshes_.at(path);
}

std::shared_ptr<Texture_2D> Assets::texture(const std::string &path,
                                            const bool color_data,
                                            const bool mipmaps,
                                            const Texture_2D::Filter &filter,
                                            const Texture_2D::Wrap &wrap) {
  if (!path.empty()) {
    if (textures_.find(path) == textures_.end()) {
      textures_.insert(Texture_pair(path, Texture_2D::load(directory_ + path, color_data, mipmaps, filter, wrap)));
    }
    return textures_.at(path);
  }
  return Shared_texture_2D(nullptr);
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

std::string Assets::directory() const {
  return directory_;
}

void Assets::clear() {
  textures_.clear();
  meshes_.clear();
}

}
}
