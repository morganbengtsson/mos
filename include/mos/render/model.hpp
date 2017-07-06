#ifndef MOS_MODEL_H
#define MOS_MODEL_H

#include <memory>
#include <optional.hpp>
#include <mos/render/material.hpp>
#include <mos/render/mesh.hpp>
#include <mos/render/texture_2d.hpp>
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
   * @param normalmap
   */
  Model(const std::string &name, const SharedMesh &mesh,
        const glm::mat4 &transform = glm::mat4(1.0f),
        const Material &material = Material(
            Material(glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(0.0f))));

  Model(const std::string &path);

  Model(const std::string &path, const json &value);

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
   * @brief material Note this is not shared.
   */
  Material material;

  /**
   * The transform of the model
   *
   * @brief transform
   */
  glm::mat4 transform;


  /**
   * @brief Children.
   */
  Models models;

private:
  std::string name_;
};
}

#endif /* MOS_MODEL_H */
