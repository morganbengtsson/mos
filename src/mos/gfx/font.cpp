#include <json.hpp>
#include <mos/gfx/font.hpp>
#include <mos/util.hpp>
#include <filesystem/path.h>

namespace mos {
namespace gfx {
using namespace nlohmann;
Font::Font(const Font::Char_map &characters, const Shared_texture_2D &texture,
           const float height)
    : texture(texture),
      characters(characters),
      height_(height) {
  texture->wrap = Texture_2D::Wrap::Clamp;
}

Font::Font(const std::string &path) {
  filesystem::path fpath = path;
  auto doc = json::parse(text(fpath.str()));
  for (auto &c : doc["symbols"]) {
    Character character;
    character.x_offset = c["xoffset"];
    character.y_offset = c["yoffset"];
    character.x_advance = c["xadvance"];
    character.width = c["width"];
    int id = c["id"];
    character.id = char(id);
    character.x = c["x"];
    character.y = c["y"];
    character.height = c["height"];
    characters.insert(std::pair<char, Character>(character.id, character));
  }
  base_ = doc["config"]["base"];
  height_ = doc["config"]["charHeight"];
  std::string texture_name = doc["config"]["textureFile"];
  texture = Texture_2D::load(fpath.parent_path().str() + "/" + texture_name);
}

float Font::height() const { return height_; }

float Font::base() const { return base_; }
}
}
