#include <json.hpp>
#include <mos/render/font.hpp>
#include <mos/util.hpp>
#include <filesystem/path.h>

namespace mos {
  using namespace nlohmann;
Font::Font(const Font::CharMap &characters, const SharedTexture &texture,
           const float height, const float ascender, const float descender)
    : characters(characters), texture(texture), height_(height),
      ascender_(ascender), descender_(descender) {
  texture->wrap = Texture2D::Wrap::CLAMP_TO_EDGE;
}

Font::Font(const std::string &path) {
    filesystem::path fpath = path;
    auto doc = json::parse(text(fpath.str()));
    for (auto & c : doc["chars"]){
      Character character;
      character.offset_x = c["offset_x"];
      character.offset_y = c["offset_y"];
      character.advance = c["advance"];
      character.rect_w = c["rect_w"];
      std::string id = c["id"];
      character.id = id[0];
      character.rect_x = c["rect_x"];
      character.rect_y = c["rect_y"];
      character.rect_h = c["rect_h"];
      characters.insert(std::pair<char, Character>(character.id, character));
    }
    height_ = doc["metrics"]["height"];
    ascender_ = doc["metrics"]["ascender"];
    descender_ = doc["metrics"]["descender"];
    std::string texture_name = doc["texture"]["file"];
    texture = Texture2D::load(fpath.parent_path().str() + "/" + texture_name);
}

Font::~Font() {}

float Font::height() const { return height_; }

float Font::ascender() const { return ascender_; }

float Font::descender() const { return descender_; }

}
