#pragma once

#include <array>
#include <glm/glm.hpp>
#include <string>

namespace mos::gfx {

/** Rendering camera view. */
class Camera final {
public:
  using Planes = std::array<glm::vec4, 6>;
  Camera(const glm::vec3 &position = glm::vec3(0.0F),
         const glm::vec3 &center = glm::vec3(0.0F),
         const glm::mat4 &projection = glm::mat4(1.0F),
         const glm::vec3 &up_direction = glm::vec3(0.0F, 0.0F, 1.0F));

  static auto load(const std::string &directory, const std::string &path,
                   const glm::mat4 &parent_transform = glm::mat4(1.0F))
      -> Camera;

  [[nodiscard]] auto position() const -> glm::vec3;

  /** Set position. */
  auto position(const glm::vec3 &position,
                const glm::vec3 &up_direction = glm::vec3(0.0F, 0.0F, 1.0F)) -> void;

  /** Set center/focus point. */
  auto center(const glm::vec3 &center,
              const glm::vec3 &up_direction = glm::vec3(0.0F, 0.0F, 1.0F)) -> void;

  /** Forward direction. **/
  [[nodiscard]] auto direction() const -> glm::vec3;

  [[nodiscard]] auto right() const -> glm::vec3;

  /** Set direction. */
  auto direction(const glm::vec3 &direction,
                 const glm::vec3 &up = glm::vec3(0.0F, 0.0F, 1.0F)) -> void;

  /** Get the aspect ratio. */
  [[nodiscard]] auto aspect_ratio() const -> float;

  /** Check if sphere with a radius is within camera frustum. */
  [[nodiscard]] auto in_frustum(const glm::vec3 &point, float radius) const -> bool;

  /** Get near clip plane. */
  [[nodiscard]] auto near_plane() const -> float;

  /** Get far clip plane. */
  [[nodiscard]] auto far_plane() const -> float;

  /** Get projection matrix. */
  [[nodiscard]] auto projection() const -> glm::mat4;

  /** Set projection matrix. */
  auto projection(const glm::mat4 &proj) -> void;

  /** Get view matrix */
  [[nodiscard]] auto view() const -> glm::mat4;

  /** Set view matrix */
  auto view(glm::mat4 mat) -> void;

  /** Get the vertical field of view. */
  [[nodiscard]] auto field_of_view_vertical() const -> float;

  /** Get the horizonal field of view. */
  [[nodiscard]] auto field_of_view_horizontal() const -> float;

private:
  auto calculate_view(const glm::vec3 &position, const glm::vec3 &center,
                      const glm::vec3 &up_direction) -> void;
  auto calculate_frustum() -> void;
  auto calculate_near_far() -> void;
  glm::mat4 projection_{};
  glm::mat4 view_{};
  Planes frustum_planes_;
  float near_{0.1F};
  float far_{100.0F};
};
} // namespace mos::gfx
