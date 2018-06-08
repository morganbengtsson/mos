#include <json.hpp>
#include <mos/gfx/font.hpp>
#include <mos/util.hpp>
#include <filesystem/path.h>

namespace mos {
namespace gfx {
using namespace nlohmann;
Font::Font(const Font::CharMap &characters, const SharedTexture2D &texture,
           const float height, const float ascender, const float descender)
    : characters(characters), texture(texture), height_(height),
      ascender_(ascender), descender_(descender) {
  texture->wrap = Texture2D::Wrap::CLAMP_TO_EDGE;
}

Font::Font(const std::string &path) {
  filesystem::path fpath = path;
  auto doc = json::parse(text(fpath.str()));
  for (auto &c : doc["symbols"]) {
    Character character;
    character.offset_x = c["xoffset"];
    character.offset_y = c["yoffset"];
    character.advance = c["xadvance"];
    character.rect_w = c["width"];
    int id = c["id"];
    character.id = char(id);
    character.rect_x = c["x"];
    character.rect_y = c["y"];
    character.rect_h = c["height"];
    characters.insert(std::pair<char, Character>(character.id, character));
  }
  height_ = doc["config"]["charHeight"];
  std::string texture_name = doc["config"]["textureFile"];
  texture = Texture2D::load(fpath.parent_path().str() + "/" + texture_name);
}

Font::~Font() {}

float Font::height() const { return height_; }
}
}
