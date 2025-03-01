#pragma once

#include <mos/core/container.hpp>
#include <mos/core/tracked_container.hpp>
#include <mos/gfx/point.hpp>
#include <mos/gfx/shape.hpp>
#include <mos/gfx/texture_2d.hpp>

namespace mos::gfx {

/** Collection of particles for rendering, uses same texture. */
class Cloud final : public Shape {
public:
  enum class Blending { Additive, Transparent };
  using Points = Tracked_container<Point>;
  Cloud(Shared_texture_2D texture, Points points);
  Cloud() = default;

  /** Sort points relative to a position. */
  auto sort(const glm::vec3 &position) -> void;

  /** Texture for all particles. */
  Shared_texture_2D texture;

  /** If the particles are emissive or diffuse.*/
  bool emissive{false};

  /** Points. */
  Points points;

  /** Blending for renderinge. */
  Blending blending = Blending::Additive;
};
} // namespace mos::gfx
