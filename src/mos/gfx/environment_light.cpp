#include <mos/gfx/environment_light.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <mos/util.hpp>
#include <filesystem>
#include <json.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace mos {
namespace gfx {

Environment_light::Environment_light(const glm::vec3 &position,
                                   const glm::vec3 &extent,
                                   const float strength,
                                   const float falloff)
    :
      strength(strength),
      falloff(falloff),
      box_(glm::translate(glm::mat4(1.0f), position), extent),
      cube_camera_(position, 1.0f, glm::length(extent)) {
}

Environment_light::Environment_light(const std::string &directory, const std::string &path,
    const glm::mat4 &parent_transform) {
  using json = nlohmann::json;
  std::filesystem::path fpath = path;

  if (fpath.extension() == ".environment_light") {
    auto value = json::parse(mos::text(directory + fpath.generic_string()));

    auto transform = parent_transform * jsonarray_to_mat4(value["transform"]);

    auto position = glm::vec3(transform[3]);

    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(transform, scale, rotation, translation, skew, perspective);

    auto extent = float(value["size"]) * scale;
    box_ = mos::gfx::Box(glm::translate(glm::mat4(1.0f), position), extent);
    strength = value.value("intensity", 1.0f);
    falloff = value.value("falloff", 0.1f);
    auto near_plane = value.value("near", 0.01f);
    auto far_plane = value.value("far", glm::length(extent));

    cube_camera_ = mos::gfx::Cube_camera(position, near_plane, far_plane);
  } else {
    throw std::runtime_error(path.substr(path.find_last_of('.')) +
        " file format is not supported.");
  }
}

void Environment_light::position(const glm::vec3 &position) {
  box_.transform[3] = glm::vec4(position, 0.0f);
  cube_camera_.position(position);
}
auto Environment_light::position() const -> glm::vec3 {
  return glm::vec3(box_.transform[3]);
}
void Environment_light::extent(const glm::vec3 &extent) {
  box_.extent = extent;
  cube_camera_.near_far(glm::length(extent) / 2.0f, glm::length(extent));
  cube_camera_.near_far(0.1f, 10.0f);
}
auto Environment_light::extent() const -> glm::vec3 {
  return box_.extent;
}
auto Environment_light::inside(const glm::vec3 &point) const -> bool {
  return box_.inside(point);
}
auto Environment_light::camera(const size_t index) const -> Camera {
  return cube_camera_.cameras.at(index);
}

}
}
