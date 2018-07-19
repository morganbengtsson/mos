#pragma once
#include <mos/gfx/model.hpp>
#include <mos/gfx/mesh.hpp>
#include <mos/gfx/character.hpp>
#include <mos/gfx/font.hpp>
#include <map>
#include <memory>
#include <string>

namespace mos {
namespace gfx {

/** Text for rendering. */
class Text {
public:
  using CharMap = std::map<char, Character>;
  using TexPtr = std::shared_ptr<Texture2D>;

  Text(const std::string &text,
       const Font &font,
       const glm::mat4 &transform = glm::mat4(1.0f),
       const bool emissive = false,
       const float spacing = 0.0f);

  virtual ~Text();

  /** Set text. */
  void text(const std::string &text);

  /** Get text. */
  std::string text() const;

  /** Approximate width. */
  float width() const;

  /** Approximate height. */
  float height() const;

  /** Set position */
  void position(const glm::vec2 &position);

  /** Set position */
  void position(const glm::vec3 &position);

  /** Get position. */
  glm::vec2 position() const;

  void scale(const float scale);

  /** Set material. */
  void material(const Material &material);

  /** Set transform */
  void transform(const glm::mat4 &transform);

  /** Get transform */
  glm::mat4 transform() const;

  /** Get model. */
  Model model() const;

  /** Set if the text is emissive. */
  void emissive(const bool emissive);

  /** Set the color multiplier for the text*/
  void factor(const glm::vec3& factor);

  /** Set opacity of text. */
  void opacity(const float &opacity);

  /** Set emission strength */
  void emission_strength(const float &strength);

  float spacing;

  Text &operator=(const std::string &text);

  Text &operator+=(const std::string &text);

public:
  Model model_;
  std::string text_;
  Font font_;
};
}
}