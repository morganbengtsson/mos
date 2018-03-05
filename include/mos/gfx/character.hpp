#pragma once

namespace mos {
namespace gfx {

/** Measurements of a character, for text rendering. */
struct Character {
  float offset_x;
  float offset_y;
  float advance;
  float rect_w;
  float rect_x;
  float rect_y;
  float rect_h;
  char id;
};
}
}
