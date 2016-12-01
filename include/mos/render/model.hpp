#ifndef MOS_MODEL_H
#define MOS_MODEL_H

#include <memory>
#include <optional.hpp>
#include <mos/render/material.hpp>
#include <mos/render/mesh.hpp>
#include <mos/render/texture.hpp>
#include <json.hpp>

namespace mos {
  using namespace nlohmann;
/**
 * A class that gathers what is needed to render an object. A Mesh, a Texture2D
 * , a Texture2D lightmap, a Texture2D normalmap and a Material.
 * Also a transformation (position/rotation).
 */
class Model {
public:
  using Models = std::vector<Model>;

  /**
   * @brief Model
   */
  Model();

  /**
   * @brief Model constructor.
   * @param mesh
   * @param texture
   * @param transform
   * @param material
   * @param lightmap
   * @param normalmap
   */
  Model(const std::string &name, const SharedMesh &mesh,
        const SharedTexture &texture = SharedTexture(),
        const glm::mat4 &transform = glm::mat4(1.0f),
        const SharedMaterial &material = std::make_shared<Material>(
            Material(glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(1.0f))),
        const SharedTexture &lightmap = SharedTexture(),
        const SharedTexture &normalmap = SharedTexture(),
        const float lit = true);

  Model(const std::string &directory, const std::string &file);

  Model(const std::string &directory, const json &value);

  /*
  Model(const mos::Model &model, const glm::mat4 transform);
  */

  /**
   * @brief ~Model destructor
   */
  ~Model();

  /**
   * @brief name
   * @return
   */
  std::string name() const;

  /**
   * Get the position
   * @brief position
   */
  glm::vec3 position() const;

  /**
  * Set the position.
  * @brief position
  * @param position
  */
  void position(const glm::vec3 &position);

  glm::vec4 overlay() const;
  void overlay(const glm::vec4 &overlay);
  void overlay(const glm::vec3 &color, float alpha);

  glm::vec3 multiply() const;
  void multiply(const glm::vec3 &multiply);

  /**
  * @brief Get the color, (to be deprecated).
  */
  glm::vec4 color() const;

  /**
   * @brief begin iterator for children models.
   * @return
   */
  Models::const_iterator begin() const;

  /**
   * @brief end iterator for children.
   * @return
   */
  Models::const_iterator end() const;

  /**
   * @brief begin iterator for children.
   * @return
   */
  Models::iterator begin();

  /**
   * @brief end iterator for children.
   * @return
   */
  Models::iterator end();

  /**
   * @brief mesh
   */
  SharedMesh mesh;

  /**
   * @brief Collection of textures.
   */
  SharedTexture texture;

  /**
   * @brief Collection of lightmaps and how they are mixed.
   */
  SharedTexture lightmap;

  /**
   * @brief normalmap
   */
  SharedTexture normalmap;

  /**
   * @brief material
   */
  SharedMaterial material;

  /**
   * The transform of the model
   *
   * @brief transform
   */
  glm::mat4 transform;

  /**
   * @brief If the model is affected by light model (lightmap and dynamic).
   */
  bool lit;

  /**
   * @brief Children.
   */
  Models models;

private:
  std::string name_;

  /**
   * @brief overlay color that is not in light calculations.
   */
  glm::vec4 overlay_;

  /**
   * @brief Extra multiply factor for light calculations.
   */
  glm::vec3 multiply_;
};
}

#endif /* MOS_MODEL_H */
