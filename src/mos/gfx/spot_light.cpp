#include <mos/gfx/spot_light.hpp>
#include <glm/gtx/transform.hpp>
#include <filesystem>
#include <json.hpp>
#include <mos/util.hpp>

namespace mos::gfx {
Spot_light::Spot_light(const glm::vec3 &position,
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

void Spot_light::position(const glm::vec3 &position) {
  camera.position(position);
}

auto Spot_light::position() const -> glm::vec3 {
  return camera.position();
}

auto Spot_light::angle(const float angle) -> void {
  angle_ = angle;
  camera.projection(glm::perspective(angle_, 1.0f, near_, far_));
}

auto Spot_light::angle() const -> float {
  return angle_;
}

auto Spot_light::direction(const glm::vec3 &direction) -> void {
  camera.direction(direction);
}

auto Spot_light::direction() const -> glm::vec3 {
  return camera.direction();
}

void Spot_light::near_far(const float near_plane, const float far_plane) {
  near_ = near_plane;
  far_ = far_plane;
  camera.projection(glm::perspective(angle_, 1.0f, near_, far_));
}

auto Spot_light::load(const std::string &directory, const std::string &path, const glm::mat4 &parent_transform) -> Spot_light {
    using json = nlohmann::json;
    if (!path.empty()) {
      std::filesystem::path fpath = path;
      auto value = json::parse(mos::text(directory + fpath.generic_string()));

      auto transform = parent_transform * jsonarray_to_mat4(value["transform"]);
      auto position = glm::vec3(transform[3]);
      auto center = position + glm::vec3(transform * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));

      std::string t = value["light"];
      auto data_value = json::parse(mos::text(directory + t));

      auto color = glm::vec3(data_value["color"][0],
                        data_value["color"][1],
                        data_value["color"][2]);
      auto strength = data_value["strength"];
      auto angle = data_value["size"];
      auto near = data_value["near"];
      auto far = data_value["far"];
      auto blend = data_value["blend"];

      return Spot_light(position, center, angle, color, strength, near, far, blend);
    } else {
      throw std::runtime_error("Invalid path");
    }
}

void Spot_light::blend(float blend) {
  blend_ = blend;
}

auto Spot_light::blend() const -> float {
  return blend_;
}
}
