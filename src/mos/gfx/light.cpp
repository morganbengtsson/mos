#include <mos/gfx/light.hpp>
#include <glm/gtx/transform.hpp>
#include <filesystem>
#include <json.hpp>
#include <mos/util.hpp>

namespace mos::gfx {
Light::Light(const glm::vec3 &position,
             const glm::vec3 &center,
             const float angle,
             const glm::vec3 &color,
             const float strength,
             const float near_plane,
             const float far_plane,
             const float blend)
    : color(color),
      strength(strength),
      camera(position, center, glm::perspective(angle, 1.0f, near_plane, far_plane),
             glm::vec3(0.0f, 0.0001f, 1.0f)),
      angle_(angle),
      near_(near_plane),
      far_(far_plane),
      blend_(blend) {}

void Light::position(const glm::vec3 &position) {
  camera.position(position);
}

auto Light::position() const -> glm::vec3 {
  return camera.position();
}

auto Light::angle(const float angle) -> void {
  angle_ = angle;
  camera.projection(glm::perspective(angle_, 1.0f, near_, far_));
}

auto Light::angle() const -> float {
  return angle_;
}

auto Light::center(const glm::vec3 &center) -> void {
  camera.center(center);
}

auto Light::center() const -> glm::vec3 {
  return camera.center();
}

auto Light::direction() const -> glm::vec3 {
  return camera.direction();
}

void Light::near_far(const float near_plane, const float far_plane) {
  near_ = near_plane;
  far_ = far_plane;
  camera.projection(glm::perspective(angle_, 1.0f, near_, far_));
}
Light::Light(const std::string &directory,
             const std::string &path,
             const glm::mat4 &parent_transform) :
  color(glm::vec3{0.0f}) {
  using json = nlohmann::json;
  if (!path.empty()) {
    std::filesystem::path fpath = path;
      auto value = json::parse(mos::text(directory + fpath.generic_string()));

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
      near_ = data_value["near"];
      far_ = data_value["far"];
      blend_ = data_value["blend"];

      static constexpr float aspect_ratio{1.0f};
      static constexpr glm::vec3 up{0.0f, 0.0001f, 1.0f};
      camera = mos::gfx::Camera(position, center, glm::perspective(angle_, aspect_ratio, near_, far_), up);

    }
}

void Light::blend(float blend) {
  blend_ = blend;
}

auto Light::blend() const -> float {
  return blend_;
}
}
