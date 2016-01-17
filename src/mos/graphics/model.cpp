/*
 * File:   Model.cpp
 * Author: morgan
 *
 * Created on March 5, 2014, 10:29 PM
 */

#include <mos/graphics/model.hpp>

namespace mos {

Model::Model() {}

Model::Model(const std::string &name, const std::shared_ptr<Mesh> &mesh,
             const std::shared_ptr<Texture2D> &texture,
             const std::shared_ptr<Texture2D> &texture2,
             const glm::mat4 &transform, const Draw draw,
             const std::shared_ptr<Material> &material,
             const std::shared_ptr<Texture2D> &lightmap,
             const std::shared_ptr<Texture2D> &normalmap,
             const float affected_by_light, const Shader shader)
    :

      draw(draw),
      mesh(mesh), texture(texture), texture2(texture2), material(material),
      lightmap(lightmap), normalmap(normalmap), name_(name),
      transform_(transform), receives_light(affected_by_light), shader(shader) {
}

Model::~Model() {}

std::string Model::name() const { return name_; }

glm::mat4 Model::transform() const { return transform_; }

void Model::transform(const glm::mat4 &transform) { transform_ = transform; }

Model::Models::const_iterator Model::begin() const { return models.begin(); }

Model::Models::const_iterator Model::end() const { return models.end(); }

Model::Models::iterator Model::begin() { return models.begin(); }

Model::Models::iterator Model::end() { return models.end(); }

glm::vec4 Model::color() const {
  return glm::vec4(material->diffuse, material->opacity);
}

glm::vec3 Model::position() const {
  auto t = transform();
  return glm::vec3(t[3][0], t[3][1], t[3][2]);
}

void Model::position(const glm::vec3 &position) {
  auto t = transform();
  t[3][0] = position[0];
  t[3][1] = position[1];
  t[3][2] = position[2];
  transform(t);
}
}
