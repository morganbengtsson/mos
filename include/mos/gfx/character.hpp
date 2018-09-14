#pragma once

namespace mos {
namespace gfx {

/** Measurements of a character glyph, for text rendering. */
struct Character final {
  float x_offset;
  float y_offset;
  float x_advance;
  float width;
  float x;
  float y;
  float height;
  char id;
};
}
}
