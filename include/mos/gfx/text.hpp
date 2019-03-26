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
class Text final {
public:
  Text(const std::string &text,
       const Font &font,
       const glm::mat4 &transform = glm::mat4(1.0f),
       float spacing = 0.0f,
       float opacity = 1.0f,
       float roughness = 1.0f,
       float metallic = 0.0f,
       float emission = 0.0f);

  ~Text() = default;

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

  /** Set scale. */
  void scale(float scale);

  /** Set transform */
  void transform(const glm::mat4 &transform);

  /** Get transform */
  glm::mat4 transform() const;

  /** Get model. */
  Model model() const;

  /** Set if the text is emissive. */
  void emission(float emission);

  /** Set opacity of text material. */
  void opacity(const float &opacity);

  /** Get the font. */
  Font font() const;

  float spacing;

  Text &operator=(const std::string &text);

  Text &operator+=(const std::string &text);

private:
  Model model_;
  std::string text_;
  Font font_;
};
}
}
