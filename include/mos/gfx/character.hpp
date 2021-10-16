#pragma once

#include <glm/glm.hpp>

namespace mos::gfx {

/** Measurements of a character glyph, for text rendering. */
struct Character final {
  /** Unicode character point id. **/
  const unsigned int id;
  const glm::vec2 position;
  const glm::vec2 offset;
  const float advance;
  const float width;
  const float height;
};
} // namespace mos::gfx
