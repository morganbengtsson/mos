#include <mos/gfx/environment_light.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <mos/util.hpp>
#include <filesystem/path.h>
#include <json.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace mos {
namespace gfx {

Environment_light::Environment_light(const glm::vec3 &position,
                                   const glm::vec3 &extent,
                                   const float strength)
    :
      strength(strength),
      box_(glm::translate(glm::mat4(1.0f), position), extent),
      cube_camera_(position, length(extent) / 2.0f, glm::length(extent)) {
}

Environment_light::Environment_light(const std::string &directory, const std::string &path,
    const glm::mat4 &parent_transform) {
  using json = nlohmann::json;
  filesystem::path fpath = path;

  if (fpath.extension() == "environment_light") {
    auto value = json::parse(mos::text(directory + fpath.str()));

    auto transform = parent_transform * jsonarray_to_mat4(value["transform"]);

    auto position = glm::vec3(transform[3]);

    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(transform, scale, rotation, translation, skew, perspective);

    auto extent = float(value["extent"]) * scale;
    box_ = mos::gfx::Box(glm::translate(glm::mat4(1.0f), position), extent);
    strength = value.value("strength", 1.0f);
    cube_camera_ = mos::gfx::Cube_camera(position, 0.01f, glm::length(extent));
  } else {
    throw std::runtime_error(path.substr(path.find_last_of('.')) +
        " file format is not supported.");
  }
}

void Environment_light::position(const glm::vec3 &position) {
  box_.transform[3] = glm::vec4(position, 0.0f);
  cube_camera_.position(position);
}
glm::vec3 Environment_light::position() const {
  return glm::vec3(box_.transform[3]);
}
void Environment_light::extent(const glm::vec3 &extent) {
  box_.extent = extent;
  cube_camera_.near_far(glm::length(extent) / 2.0f, glm::length(extent));
}
glm::vec3 Environment_light::extent() const {
  return box_.extent;
}
bool Environment_light::inside(const glm::vec3 &point) const {
  return box_.inside(point);
}
mos::gfx::Camera Environment_light::camera(const size_t index) const {
  return cube_camera_.cameras.at(index);
}

}
}
