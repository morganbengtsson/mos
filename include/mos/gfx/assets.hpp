#pragma once


#include <map>
#include <memory>

#include <json.hpp>

#include <mos/gfx/character.hpp>
#include <mos/gfx/environment_light.hpp>
#include <mos/gfx/font.hpp>
#include <mos/gfx/mesh.hpp>
#include <mos/gfx/model.hpp>
#include <mos/gfx/spot_light.hpp>
#include <mos/gfx/texture_2d.hpp>
#include <unordered_map>

namespace mos::gfx {

/** Cache for faster loading of textures and meshes. */
class Assets final {
public:
  using Mesh_map = std::unordered_map<std::string, std::shared_ptr<Mesh>>;
  using Texture_map =
      std::unordered_map<std::string, std::shared_ptr<Texture_2D>>;
  using Mesh_pair = std::pair<std::string, std::shared_ptr<Mesh>>;
  using Texture_pair = std::pair<std::string, std::shared_ptr<Texture_2D>>;

  /** @param directory The directory where the assets exist, relative to the run
   * directory. */
  explicit Assets(std::string directory = "assets/");

  Assets(const Assets &assets) = delete;
  Assets(const Assets &&assets) = delete;
  Assets &operator=(const Assets &assets) = delete;
  Assets &operator=(const Assets &&assets) = delete;
  ~Assets() = default;

  /** Loads a Mesh from a *.mesh file and caches it internally. */
  auto mesh(const std::string &path) -> Shared_mesh;

  /** Loads Texture2D from a *.png file or *.texture and caches it internally.
   */
  auto texture(const std::string &path, bool color_data = true,
               bool mipmaps = true,
               const Texture_2D::Filter &filter = Texture_2D::Filter::Linear,
               const Texture_2D::Wrap &wrap = Texture_2D::Wrap::Repeat)
      -> Shared_texture_2D;

  /** Remove all unused assets. */
  auto clear_unused() -> void;

  /** Clear all assets. */
  auto clear() -> void;

  /** Parent directory of the cache. */
  auto directory() const -> std::string;

private:
  const std::string directory_;
  Mesh_map meshes_;
  Texture_map textures_;
};
} // namespace mos::gfx
