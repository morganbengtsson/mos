#ifndef MOS_ASSETS_H
#define MOS_ASSETS_H

#include <json.hpp>
#include <map>
#include <memory>
#include <mos/render/animation.hpp>
#include <mos/render/character.hpp>
#include <mos/render/font.hpp>
#include <mos/render/material.hpp>
#include <mos/render/mesh.hpp>
#include <mos/render/model.hpp>
#include <mos/render/texture_2d.hpp>
#include <unordered_map>

namespace mos {
/**
 * An asset class that manages heavy resources such as Textures, meshes, sounds
 * and sound streams.
 * Caches most things internally, so nothing is loaded twice.
 */
class RenderAssets final {
public:
  /**
   * @brief Container for meshes.
   */
  using MeshMap = std::unordered_map<std::string, std::shared_ptr<Mesh>>;

  /**
   * @brief Container for textures.
   */
  using TextureMap = std::unordered_map<std::string, std::shared_ptr<Texture2D>>;

  /**
   * @brief Pair for MeshMap.
   */
  using MeshPair = std::pair<std::string, std::shared_ptr<Mesh>>;

  /**
   * @brief Pair for TextureMap
   */
  using TexturePair = std::pair<std::string, std::shared_ptr<Texture2D>>;

  /**
  * @brief Pair for MaterialMap.
  */
  using MaterialPair = std::pair<std::string, std::shared_ptr<Material>>;

  using SharedMaterial = std::shared_ptr<mos::Material>;


  /**
   * Constructor for the asset manager.
   *
   * @brief Assets
   * @param directory The directory where the assets exist, relative to the run
   *directory, default is "assets/"
   */
  RenderAssets(const std::string directory = "assets/");

  /**
   * @brief Assets non copyable.
   * @param assets
   */
  RenderAssets(const RenderAssets &assets) = delete; // Not copyable.

  /**
   * @brief Destructor
   */
  ~RenderAssets();

  /**
   * @brief Loads a full Model from a *.model file.
   * @param path
   * @return Model object including associated meshes/textures/etc.
   */
  Model model(const std::string &path);

  /**
   * @brief Loads an animation from meshes specified in *.animation file.
   * @param path
   * @return Animation object
   */
  Animation animation(const std::string &path);

  /**
   * Loads a *.material file into a Material object, and caches it internally.
   *
   * @param path
   * @return fShared pointer to Material object.
   */
   Material material(const std::string &path);

  /**
   * Loads a *.obj or *.mesh file into a mesh object, and caches it internally.
   *
   * @param path
   * @return Shared pointer to Mesh object.
   */
  SharedMesh mesh(const std::string &path);

  /**
   * Loads a *.png file into a Texture2D object, and caches it internally.
   *
   * @param path
   * @param mipmaps Use mipmaps or not.
   * @return Shared pointer to Texture2D object.
   */
  SharedTexture2D
  texture(const std::string &path, const bool mipmaps = true,
                 const bool compress = true,
                 const Texture2D::Wrap &wrap = Texture2D::Wrap::REPEAT);




  /**
    * @brief Remove all unused assets.
    */
  void clear_unused();

private:
  // Model model(rapidjson::Value &value);
  Model model_value(const std::string &base_path, const nlohmann::json &value);
  const std::string directory_;
  MeshMap meshes_;
  TextureMap textures_;
};
}
#endif /* MOS_ASSETS_H */
