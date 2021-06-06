#include <json.hpp>
#include <mos/gfx/font.hpp>
#include <mos/util.hpp>
#include <filesystem>

namespace mos::gfx {
using namespace nlohmann;
Font::Font(const Font::Char_map &characters,
           const Shared_texture_2D &texture,
           const float height,
           const float base)
    : texture(texture),
      characters(characters),
      height_(height),
      base_(base) {
  texture->wrap = Texture_2D::Wrap::Clamp;
}

Font Font::load(const std::string &path) {
  std::filesystem::path fpath = path;
  Char_map characters;
  float base, height;
  Shared_texture_2D texture;

  auto doc = json::parse(text(fpath.generic_string()));
  for (auto &c : doc["symbols"]) {
    Character character{
        .id = c["id"],
        .position = {c["x"], c["y"]},
        .offset = {c["xoffset"], c["yoffset"]},
        .advance = c["xadvance"],
        .width = c["width"],
        .height = c["height"],
        };
    characters.insert(Char_pair(character.id, character));
  }
  base = doc["config"]["base"];
  height = doc["config"]["charHeight"];
  std::string texture_name = doc["config"]["textureFile"];
  texture = std::make_shared<Texture_2D>(Texture_2D::load(fpath.parent_path().generic_string() + "/" + texture_name));
  return Font(characters, texture, height, base);
}

auto Font::height() const -> float { return height_; }

auto Font::base() const -> float { return base_; }
}
