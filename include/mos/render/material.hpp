#ifndef MOS_MATERIAL_H
#define MOS_MATERIAL_H

#include <glm/glm.hpp>
#include <string>
#include <memory>
#include <mos/render/texture_2d.hpp>
#include <mos/render/texture_cube.hpp>

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
   * @param normal_map
   * @param diffuse_environment_map
   * @param specular_environment_map
   * @param ambient
   * @param diffuse
   * @param specular
   * @param opacity
   * @param shininess
   */
  explicit Material(const SharedTexture2D &diffuse_map = SharedTexture2D(),
                    const SharedTexture2D &emmision_map = SharedTexture2D(),
                    const SharedTexture2D &normal_map = SharedTexture2D(),
                    const SharedTexture2D &light_map = SharedTexture2D(),
                    const glm::vec3 &ambient = glm::vec3(0.0f),
                    const glm::vec3 &diffuse = glm::vec3(1.0f),
                    const glm::vec3 &specular = glm::vec3(0.0f),
                    const glm::vec3 &emission = glm::vec3(0.0f),
                    const float opacity = 1.0f,
                    const float shininess = 0.0f);

  explicit Material(const glm::vec3 &ambient,
                    const glm::vec3 &diffuse,
                    const glm::vec3 &specular,
                    const glm::vec3 &emission,
                    const float opacity = 1.0f,
                    const float shininess = 0.0f);

  /**
   * @brief Material construction from *.mesh file.
   * @param path
   */
  explicit Material(const std::string &path);

  virtual ~Material();

  static Material load(const std::string &path);

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
   * @brief emission color.
   */
  glm::vec3 emission;

  /**
   * @brief opacity
   */
  float opacity;

  /**
   * @brief shininess
   *
   * In some equations also called Ns.
   */
  float shininess;

  /**
  * @brief Texture of the material.
  */
  SharedTexture2D diffuse_map;

  /**
  * @brief Texture of the material.
  */
  SharedTexture2D emission_map;

  /**
  * @brief Normalmap for the material.
  */
  SharedTexture2D normal_map;

  /**
  * @brief Light map for the material.
  */
  SharedTexture2D light_map;

};
}

#endif /* MOS_MATERIAL_H */
