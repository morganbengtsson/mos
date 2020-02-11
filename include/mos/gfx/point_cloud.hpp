#pragma once
#include <vector>
#include <atomic>
#include <chrono>
#include <mos/core/tracked_container.hpp>
#include <mos/core/container.hpp>
#include <mos/gfx/texture_2d.hpp>
#include <mos/gfx/shape.hpp>
#include <mos/gfx/point.hpp>

namespace mos {
namespace gfx {

/** Collection of particles for rendering, uses same texture. */
class Point_cloud final : public Shape {
public:
  using Points = Tracked_container<Point>;
  Point_cloud(Shared_texture_2D texture, Points particles);
  Point_cloud() = default;

  /** Sort particles relative to a position. */
  void sort(const glm::vec3 &position);

  /** Texture for all particles. */
  Shared_texture_2D texture;

  /** Particles. */
  Points points;
};
}
}
