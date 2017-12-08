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
   * @param ambient
   * @param albedo
   * @param opacity
   * @param roughness
   */
  explicit Material(const SharedTexture2D &albedo_map = SharedTexture2D(),
                    const SharedTexture2D &normal_map = SharedTexture2D(),
                    const SharedTexture2D &light_map = SharedTexture2D(),
                    const glm::vec3 &albedo = glm::vec3(1.0f),
                    const float opacity = 1.0f,
                    const float roughness = 0.0f,
                    const float metallic = 0.0f);

  explicit Material(const glm::vec3 &diffuse,
                    const float opacity = 1.0f,
                    const float roughness = 0.0f,
                    const float metallic = 0.0f);

  /**
   * @brief Material construction from *.mesh file.
   * @param path
   */
  explicit Material(const std::string &path);

  virtual ~Material();

  static Material load(const std::string &path);

  /**
   * @brief diffuse color.
   */
  glm::vec3 albedo;

  /**
   * @brief opacity
   */
  float opacity;

  /**
   * @brief roughness
    */
  float roughness;

  /**
   * @brief metallic
    */
  float metallic;

  /**
  * @brief Texture of the material.
  */
  SharedTexture2D albedo_map;

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
