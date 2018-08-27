#pragma once

#include <map>
#include <memory>
#include <unordered_map>
#include <json.hpp>
#include <mos/gfx/character.hpp>
#include <mos/gfx/font.hpp>
#include <mos/gfx/mesh.hpp>
#include <mos/gfx/model.hpp>
#include <mos/gfx/texture_2d.hpp>
#include <mos/gfx/light.hpp>
#include <mos/gfx/environment_light.hpp>

namespace mos {
namespace gfx {

/** Cache for faster loading of textures and meshes. */
class Assets final {
public:
  using MeshMap = std::unordered_map<std::string, std::shared_ptr<Mesh>>;
  using TextureMap = std::unordered_map<std::string, std::shared_ptr<Texture2D>>;
  using MeshPair = std::pair<std::string, std::shared_ptr<Mesh>>;
  using TexturePair = std::pair<std::string, std::shared_ptr<Texture2D>>;

  /** @param directory The directory where the assets exist, relative to the run directory. */
  explicit Assets(const std::string &directory = "assets/");

  Assets(const Assets &assets) = delete;

  ~Assets() = default;

  /** Loads a Mesh from a *.mesh file and caches it internally. */
  SharedMesh mesh(const std::string &path);

  /** Loads Texture2D from a *.png file and caches it internally. */
  SharedTexture2D
  texture(const std::string &path,
          bool color_data = true,
          bool mipmaps = true,
          const Texture2D::Wrap &wrap = Texture2D::Wrap::REPEAT);

  /** Remove all unused assets. */
  void clear_unused();

  /** Clear all assets. */
  void clear();

  std::string directory() const;
private:
  const std::string directory_;
  MeshMap meshes_;
  TextureMap textures_;
};
}
}