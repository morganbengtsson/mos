#ifndef MOS_ASSETS_H
#define MOS_ASSETS_H

#include <unordered_map>
#include <map>
#include <memory>
#include <json.hpp>
#include <mos/audio/sound.hpp>
#include <mos/audio/stream.hpp>
#include <mos/graphics/character.hpp>
#include <mos/graphics/mesh.hpp>
#include <mos/graphics/texture.hpp>
#include <mos/graphics/material.hpp>
#include <mos/graphics/model.hpp>
#include <mos/graphics/font.hpp>
#include <mos/graphics/animation.hpp>

namespace mos {
/**
 * An asset class that manages heavy resources such as Textures, meshes, sounds
 * and sound streams.
 * Caches most things internally, so nothing is loaded twice.
 */
class Assets {
public:
  /**
   * @brief Container for meshes.
   */
  using MeshMap = std::unordered_map<std::string, std::shared_ptr<Mesh>>;

  /**
   * @brief Container for textures.
   */
  using TextureMap =
      std::unordered_map<std::string, std::shared_ptr<Texture>>;

  /**
   * @brief Container for sounds.
   */
  using SoundMap = std::unordered_map<std::string, std::shared_ptr<Sound>>;

  /**
   * @brief Container for materials.
   */
  using MaterialMap =
      std::unordered_map<std::string, std::shared_ptr<Material>>;

  /**
   * @brief Pair for MeshMap.
   */
  using MeshPair = std::pair<std::string, std::shared_ptr<Mesh>>;

  /**
   * @brief Pair for TextureMap
   */
  using TexturePair = std::pair<std::string, std::shared_ptr<Texture>>;

  /**
   * @brief Pair for SoundMap
   */
  using SoundPair = std::pair<std::string, std::shared_ptr<Sound>>;

  /**
  * @brief Pair for MaterialMap.
  */
  using MaterialPair = std::pair<std::string, std::shared_ptr<Material>>;

  /**
   * Constructor for the asset manager.
   *
   * @brief Assets
   * @param directory The directory where the assets exist, relative to the run
   *directory, default is "assets/"
   */
  Assets(const std::string directory = "assets/");

  /**
   * @brief Assets non copyable.
   * @param assets
   */
  Assets(const Assets &assets) = delete; // Not copyable.
  virtual ~Assets();

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
  std::shared_ptr<Material> material_cached(const std::string &path);

  /**
   *
   * Loads  a *.material file into a Material object. Not cached.
   *
   * @brief material
   * @param path
   * @return Shared pointer to a Material object.
   */
  std::shared_ptr<Material> material(const std::string &path) const;

  /**
   * Loads a *.obj or *.mesh file into a mesh object, and caches it internally.
   *
   * @param path
   * @return Shared pointer to Mesh object.
   */
  std::shared_ptr<Mesh> mesh_cached(const std::string &path);

  /**
   * Loads a *.obj or *.mesh file into a mesh object. Not cached.
   *
   * @param path
   * @return Shared pointer to Mesh object
   */
  std::shared_ptr<Mesh> mesh(const std::string &path) const;

  /**
   * Loads a *.png file into a Texture2D object, and caches it internally.
   *
   * @param path
   * @param mipmaps Use mipmaps or not.
   * @return Shared pointer to Texture2D object.
   */
  std::shared_ptr<Texture> texture_cached(const std::string &path,
                                            const bool mipmaps = true);

  /**
   * Loads a *.png file into A Texture2D object. Not cached.
   *
   * @param path
   * @param mipmaps Use mipmaps or not.
   * @return Shared pointer to Texture2D object.
   */
  std::shared_ptr<Texture> texture(const std::string &path,
                                     const bool mipmaps = true) const;

  /**
   * Loads a *.ogg file into a Sound object, and caches it internally.
   *
   * @param path
   * @return Shared pointer to Sound object.
   */
  std::shared_ptr<Sound> sound_cached(const std::string &path);

  /**
   * Loads an *. ogg file into a Sound object. Not cached.
   *
   * @param path
   * @return Shared pointer to Sound object.
   */
  std::shared_ptr<Sound> sound(const std::string &path) const;

  /**
   * Loads an *.ogg file into a Stream object. Not cached, since it is streamed.
   *
   * @brief stream
   * @param path
   * @return
   */
  std::shared_ptr<Stream> stream(const std::string &path) const;

  /**
   * @brief Load font file in ngl format.
   * @param ngl_path

   * @return
   */
  Font font(const std::string &path);

 /**
   * @brief Remove all unused assets.
   */
  void clear_unused();

private:
  //Model model(rapidjson::Value &value);
  Model model_value(const nlohmann::json &value);
  const std::string directory_;
  MeshMap meshes_;
  TextureMap textures_;
  SoundMap sounds_;
  MaterialMap materials_;
};
}
#endif /* MOS_ASSETS_H */
