#include <json.hpp>
#include <mos/gfx/font.hpp>
#include <mos/util.hpp>
#include <filesystem>

namespace mos {
namespace gfx {
using namespace nlohmann;
Font::Font(Font::Char_map characters,
           Shared_texture_2D texture,
           const float height,
           const float base)
    : texture(std::move(texture)),
      characters(std::move(characters)),
      height_(height),
      base_(base) {
  texture->wrap = Texture_2D::Wrap::Clamp;
}

Font::Font(const std::string &path) {
  std::filesystem::path fpath = path;
  auto doc = json::parse(text(fpath.generic_string()));
  for (auto &c : doc["symbols"]) {
    Character character{};
    character.x_offset = c["xoffset"];
    character.y_offset = c["yoffset"];
    character.x_advance = c["xadvance"];
    character.width = c["width"];
    character.id = c["id"];
    character.x = c["x"];
    character.y = c["y"];
    character.height = c["height"];
    characters.insert(std::pair<unsigned int, Character>(character.id, character));
  }
  base_ = doc["config"]["base"];
  height_ = doc["config"]["charHeight"];
  std::string texture_name = doc["config"]["textureFile"];
  texture = Texture_2D::load(fpath.parent_path().generic_string() + "/" + texture_name);
}

float Font::height() const { return height_; }

float Font::base() const { return base_; }
}
}
