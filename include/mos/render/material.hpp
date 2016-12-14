#ifndef MOS_MATERIAL_H
#define MOS_MATERIAL_H

#include <glm/glm.hpp>
#include <string>
#include <memory>
#include <mos/render/texture.hpp>

namespace mos {
  class Material;
  using SharedMaterial = std::shared_ptr<Material>;

/**
 * @brief The Material class
 *
 * Aspects of a material supported by the renderer. Different from
 * texture.
 *
 */
class Material {
public:
  /**
   * @brief Material
   * @param ambient
   * @param diffuse
   * @param specular
   * @param opacity
   * @param specular_exponent
   */
  explicit Material(const SharedTexture &texture = SharedTexture(),
                    const SharedTexture &normalmap = SharedTexture(),
                    const glm::vec3 &ambient = glm::vec3(1.0f),
                    const glm::vec3 &diffuse = glm::vec3(1.0f),
                    const glm::vec3 &specular = glm::vec3(0.0f),
                    const float opacity = 1.0f,
                    const float specular_exponent = 0.0f);

  /**
   * @brief Material construction from *.mesh file.
   * @param path
   */
  explicit Material(const std::string &directory, const std::string &path);

  virtual ~Material();

  static SharedMaterial load(const std::string &directory, const std::string &path);

  /**
   * @brief ambient color.
   */
  glm::vec3 ambient;

  /**
   * @brief diffuse color.
   */
  glm::vec3 diffuse;

  /**
   * @brief specular color.
   */
  glm::vec3 specular;

  /**
   * @brief opacity
   */
  float opacity;

  /**
   * @brief specular_exponent
   *
   * In some equations also called Ns.
   */
  float specular_exponent;

  /**
  * @brief Texture of the material.
  */
  SharedTexture texture;

  /**
  * @brief Normalmap for the material.
  */
  SharedTexture normalmap;
};
}

#endif /* MOS_MATERIAL_H */
