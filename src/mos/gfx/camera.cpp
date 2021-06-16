#include <mos/gfx/camera.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/io.hpp>
#include <json.hpp>
#include <iostream>
#include <filesystem>
#include <mos/util.hpp>

namespace mos::gfx {

Camera::Camera(const glm::vec3 &position, const glm::vec3 &center,
               const glm::mat4 &projection, const glm::vec3 &up)
    : projection_(projection),
      view_(1.0f), frustum_planes_{glm::vec4(0.0f), glm::vec4(0.0f),
                                   glm::vec4(0.0f), glm::vec4(0.0f),
                                   glm::vec4(0.0f), glm::vec4(0.0f)} {
  calculate_view(position, center, up);
  calculate_frustum();
  calculate_near_far();
}

auto Camera::load(const std::string &directory, const std::string &path,
                  const glm::mat4 &parent_transform) -> Camera {
  using json = nlohmann::json;
  std::filesystem::path fpath = path;
  if (!path.empty() && fpath.extension() == ".camera") {
    auto value = json::parse(mos::text(directory + fpath.generic_string()));

    auto transform = parent_transform * jsonarray_to_mat4(value["transform"]);
    auto position = glm::vec3(transform[3]);
    auto center =
        position + glm::vec3(transform * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));

    auto proj = mos::jsonarray_to_mat4(value["projection"]);

    return Camera(position, center, proj);
  }
  throw std::runtime_error(path.substr(path.find_last_of('.')) +
                             " file format is not supported.");

}

auto Camera::position() const -> glm::vec3 { return glm::vec3(-view_[3]) * glm::mat3(view_); }

void Camera::position(const glm::vec3 &position, const glm::vec3 &up) {
  calculate_view(position, position + direction(), up);
  calculate_frustum();
}

void Camera::center(const glm::vec3 &center, const glm::vec3 &up) {
  calculate_view(position(), center, up);
  calculate_frustum();
}

auto Camera::direction() const -> glm::vec3 {
  return glm::vec3(0.0f, 0.0f, -1.0f) * glm::mat3(view_);
}

void Camera::direction(const glm::vec3 &direction, const glm::vec3 &up) {
  center(position() + direction);
}

void Camera::calculate_view(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up) {
  view_ = glm::lookAt(position, center, up);
}

auto Camera::aspect_ratio() const -> float {
  return projection_[1][1] / projection_[0][0];
}

void Camera::calculate_frustum() {
  auto m = projection_ * view_;
  auto t = glm::transpose(m);

  frustum_planes_[0] = t[3] + t[0]; /* left   */
  frustum_planes_[1] = t[3] - t[0]; /* right  */
  frustum_planes_[2] = t[3] + t[1]; /* bottom */
  frustum_planes_[3] = t[3] - t[1]; /* top    */
  frustum_planes_[4] = t[3] + t[2]; /* near   */
  frustum_planes_[5] = t[3] - t[2]; /* far    */

  for (auto & plane : frustum_planes_){
    auto magnitude = glm::length(glm::vec3(plane));
    plane /= magnitude;
  }
}

void Camera::calculate_near_far() {
  auto m = glm::transpose(projection_);

  auto n0 = m[3] + m[2];
  n0 = n0 / glm::length(glm::vec3(n0));
  auto f0 = m[3] - m[2];
  f0 = f0 / glm::length(glm::vec3(f0));

  near_ = n0.z * n0.w;
  far_ = f0.z * f0.w;
}

auto Camera::in_frustum(const glm::vec3 &point, const float radius) const
    -> bool {
  return std::none_of(frustum_planes_.begin(), frustum_planes_.end(),
                      [&](const auto &plane) {
                        return (plane.x * point.x + plane.y * point.y +
                                    plane.z * point.z + plane.w <=
                                -radius);
                      });
}

auto Camera::near_plane() const -> float {
  return near_;
}

auto Camera::far_plane() const -> float {
  return far_;
}

auto Camera::projection() const -> glm::mat4 {
  return projection_;
}

void Camera::projection(const glm::mat4 &proj) {
  projection_ = proj;
  calculate_frustum();
  calculate_near_far();
}

auto Camera::view() const -> glm::mat4 {
  return view_;
}

//TODO: Discards position_, up_ and center_ states...
auto Camera::view(const glm::mat4 mat) -> void{
  view_ = mat;
  calculate_frustum();
}

auto Camera::field_of_view_vertical() const -> float {
  return 2.0 * glm::atan(1.0f / projection()[1][1]);
}

auto Camera::field_of_view_horizontal() const -> float {
  return aspect_ratio() * field_of_view_vertical();
}

}

