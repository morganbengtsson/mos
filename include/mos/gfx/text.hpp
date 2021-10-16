#pragma once

#include <map>
#include <memory>
#include <string>

#include <mos/gfx/character.hpp>
#include <mos/gfx/font.hpp>
#include <mos/gfx/mesh.hpp>
#include <mos/gfx/model.hpp>

namespace mos::gfx {

/** Text for rendering. */
class Text final {
public:
  Text(const std::string &txt, Font font,
       const glm::mat4 &transform = glm::mat4(1.0f), float spacing = 0.0f,
       float opacity = 1.0f, float roughness = 1.0f, float metallic = 0.0f,
       const glm::vec3 &emission = glm::vec3(0.0f));

  /** Set text. */
  auto text(const std::string &text) -> void;

  /** Get text. */
  auto text() const -> std::string;

  /** Approximate width. */
  auto width() const -> float;

  /** Approximate height. */
  auto height() const -> float;

  /** Set position */
  auto position(const glm::vec2 &position) -> void;

  /** Set position */
  auto position(const glm::vec3 &position) -> void;

  /** Get position. */
  auto position() const -> glm::vec2;

  /** Set scale. */
  auto scale(float scale) -> void;

  /** Set transform */
  auto transform(const glm::mat4 &transform) -> void;

  /** Get transform */
  auto transform() const -> glm::mat4;

  /** Get model. */
  auto model() const -> Model;

  /** Set opacity. */
  auto opacity(float opacity) -> void;

  /** Set if the text is emissive. */
  auto emission(const glm::vec3 &emission) -> void;

  /** Get the font. */
  auto font() const -> Font;

  float spacing;

  auto operator=(const std::string &input) -> Text &;

  auto operator+=(const std::string &input) -> Text &;

private:
  Model model_;
  std::string text_;
  Font font_;
};
} // namespace mos::gfx
