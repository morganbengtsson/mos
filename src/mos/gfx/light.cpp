#include <mos/gfx/light.hpp>
#include <glm/gtx/transform.hpp>
#include <filesystem/path.h>
#include <json.hpp>
#include <mos/util.hpp>

namespace mos {
namespace gfx {
Light::Light(const glm::vec3 &position,
             const glm::vec3 &center,
             const float angle,
             const glm::vec3 &color,
             const float strength,
             const float near,
             const float far)
    : color(color), angle_(angle),
      camera(position, center, glm::perspective(angle, 1.0f, near, far),
             glm::vec3(0.0f, 0.0001f, 1.0f)), strength(strength), near_(near), far_(far) {}



Light::~Light() {}

void Light::position(const glm::vec3 &position) {
  camera.position(position);
}

glm::vec3 Light::position() const {
  return camera.position();
}

void Light::angle(const float angle) {
  angle_ = angle;
  camera.projection = glm::perspective(angle_, 1.0f, near_, far_);
}

float Light::angle() const {
  return angle_;
}

void Light::center(const glm::vec3 &center) {
  camera.center(center);
}

glm::vec3 Light::center() const {
  return camera.center();
}

glm::vec3 Light::direction() const {
  return camera.direction();
}

void Light::near_far(const float near, const float far) {
  near_ = near;
  far_ = far;
  camera.projection = glm::perspective(angle_, 1.0f, near_, far_);

}
Light::Light(const std::string &directory, const std::string &path, const float near, const float far,
    const glm::mat4 &parent_transform) : near_(near), far_(far) {
  using json = nlohmann::json;
  if (!path.empty()) {
    filesystem::path fpath = path;
    if (fpath.extension() == "light") {
      auto value = json::parse(mos::text(directory + fpath.str()));

      auto transform = parent_transform * jsonarray_to_mat4(value["transform"]);
      auto position = glm::vec3(transform[3]);
      auto center = position + glm::vec3(transform * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));

      std::string t = value["light"];
      auto data_value = json::parse(mos::text(directory + t));

      color = glm::vec3(data_value["color"][0],
                             data_value["color"][1],
                             data_value["color"][2]);
      strength = data_value["strength"];
      angle_ = data_value["size"];
      auto blend = value["blend"];

      camera = mos::gfx::Camera(position, center, glm::perspective(angle_, 1.0f, near, far),
             glm::vec3(0.0f, 0.0001f, 1.0f));

    } else {
      throw std::runtime_error(path.substr(path.find_last_of(".")) +
          " file format is not supported.");
    }
  }
}
}
}