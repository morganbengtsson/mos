#pragma once

namespace mos {
namespace gfx {
/**
 * @brief Measurements of a character, for text rendering.
 *
 */
struct Character {
  /**
   * @brief offset_x
   */
  float offset_x;
  /**
   * @brief offset_y
   */
  float offset_y;
  /**
   * @brief advance
   */
  float advance;
  /**
   * @brief rect_w
   */
  float rect_w;
  /**
   * @brief id
   */
  char id;
  /**
   * @brief rect_x
   */
  float rect_x;
  /**
   * @brief rect_y
   */
  float rect_y;
  /**
   * @brief rect_h
   */
  float rect_h;
};
}
}
