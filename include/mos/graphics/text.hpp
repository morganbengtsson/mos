#ifndef MOS_TEXT_H
#define MOS_TEXT_H

#include <mos/graphics/model.hpp>
#include <mos/graphics/mesh.hpp>
#include <mos/graphics/character.hpp>
#include <mos/graphics/font.hpp>

#include <map>
#include <memory>
#include <string>

namespace mos {

/**
 * A special Model class for text rendering.
 */
class Text {
public:
  using CharMap = std::map<char, Character>;
  using TexPtr = std::shared_ptr<Texture2D>;

  /**
   * @brief Text constructor
   * @param text
   * @param characters
   * @param texture with characters
   * @param transform for position/rotation
   * @param spacing between characters
   */
  Text(const std::string &text, const Font &font,
       const glm::mat4 &transform = glm::mat4(1.0f),
       const float spacing = 0.0f);
  virtual ~Text();

  /**
   * Update the text.
   * @param text String to use.
   */
  void text(const std::string &text);

  /**
   * @brief text
   * @return Current text
   */
  std::string text() const;

  /**
   * @brief approximate width
   * @return width
   */
  float width() const;

  /**
   * @brief height
   * @return
   */
  float height() const;

  /**
   * @brief position
   * @param position
   */
  void position(const glm::vec2 &position);

  /**
   * @brief position
   * @return
   */
  glm::vec2 position();

  /**
   * @brief scale
   * @param scale
   */
  void scale(const float scale);

  /**
   * @brief material
   * @param material
   */
  void material(const std::shared_ptr<Material> &material);

  /**
   * @brief transform
   * @param transform
   */
  void transform(const glm::mat4 &transform);

  /**
   * @brief transform
   * @return
   */
  glm::mat4 transform() const;

  /**
   * @brief model
   * @return
   */
  mos::Model model() const;

  /**
   * @brief spacing between characters
   */
  float spacing;

  /**
   * @brief operator =
   * @param text
   * @return
   */
  mos::Text &operator=(const std::string &text);

  /**
   * @brief operator +=
   * @param text
   * @return
   */
  mos::Text &operator+=(const std::string &text);

private:
  mos::Model model_;
  std::string text_;
  Font font_;
};
}

#endif /* MOS_TEXT_H */
