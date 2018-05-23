#pragma once

#include <map>
#include <memory>
#include <unordered_map>
#include <json.hpp>
#include <mos/gfx/animation.hpp>
#include <mos/gfx/character.hpp>
#include <mos/gfx/font.hpp>
#include <mos/gfx/material.hpp>
#include <mos/gfx/mesh.hpp>
#include <mos/gfx/model.hpp>
#include <mos/gfx/texture_2d.hpp>
#include <mos/gfx/light.hpp>
#include <mos/gfx/environment_light.hpp>

namespace mos {
namespace gfx {

/** Handles heavy render assets, such as Textures and meshes. Caches things internally, so nothing is loaded twice. */
class Assets final {
public:
  using MeshMap = std::unordered_map<std::string, std::shared_ptr<Mesh>>;
  using TextureMap = std::unordered_map<std::string, std::shared_ptr<Texture2D>>;
  using MeshPair = std::pair<std::string, std::shared_ptr<Mesh>>;
  using TexturePair = std::pair<std::string, std::shared_ptr<Texture2D>>;
  using MaterialPair = std::pair<std::string, std::shared_ptr<Material>>;
  using SharedMaterial = std::shared_ptr<Material>;

  /** @param directory The directory where the assets exist, relative to the run directory. */
  Assets(const std::string directory = "assets/");

  Assets(const Assets &assets) = delete;

  ~Assets();

  /** Loads a Model from a *.model file.*/
  Model model(const std::string &path);

  /** Loads an animation from meshes specified in *.animation file. */
  Animation animation(const std::string &path);

  /** Loads a Material from a *.material file into a Material object. */
  Material material(const std::string &path);

  /** Loads a Light from a *.light file into a Light object. */
  Light light(const std::string &path);

  /** Loads a EnvironmentLight from a *.environment_light. */
  EnvironmentLight environment_light(const std::string &path);

  /** Loads a Mesh from a *.mesh file and caches it internally. */
  SharedMesh mesh(const std::string &path);

  /** Loads Texture2D from a *.png file and caches it internally. */
  SharedTexture2D
  texture(const std::string &path,
          const bool color_data = true,
          const bool mipmaps = true,
          const Texture2D::Wrap &wrap = Texture2D::Wrap::REPEAT);

  /** Remove all unused assets. */
  void clear_unused();
  std::string directory() const;
private:
  Model model_value(const std::string &base_path, const nlohmann::json &value);
  const std::string directory_;
  MeshMap meshes_;
  TextureMap textures_;
};
}
}