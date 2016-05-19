#include <mos/simulate/box.hpp>

#include <utility>
#include <stdexcept>
#include <iostream>
#include <cassert>
#include <array>
#include <glm/gtx/io.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/component_wise.hpp>

namespace mos {

std::atomic_uint Box::current_id_(0);

Box::Box(const glm::vec3 &extent, const glm::vec3 &position)
    : extent(extent), position(position), id_(current_id_++) {}

Box::Box() {}

glm::vec3 Box::min() const { return position - extent; }
glm::vec3 Box::max() const { return position + extent; }

bool Box::intersect(const glm::vec3 &origin, const glm::vec3 &direction) const {
  float tmin, tmax, tymin, tymax, tzmin, tzmax;

  glm::vec3 bounds[2];
  bounds[0] = min();
  bounds[1] = max();

  glm::vec3 invdir = 1.f / direction;
  glm::i8vec3 sign;

  sign.x = (invdir.x < 0);
  sign.y = (invdir.y < 0);
  sign.z = (invdir.z < 0);

  tmin = (bounds[sign.x].x - origin.x) * invdir.x;
  tmax = (bounds[1 - sign.x].x - origin.x) * invdir.x;
  tymin = (bounds[sign.y].y - origin.y) * invdir.y;
  tymax = (bounds[1 - sign.y].y - origin.y) * invdir.y;

  if ((tmin > tymax) || (tymin > tmax))
    return false;
  if (tymin > tmin)
    tmin = tymin;
  if (tymax < tmax)
    tmax = tymax;

  tzmin = (bounds[sign.z].z - origin.z) * invdir.z;
  tzmax = (bounds[1 - sign.z].z - origin.z) * invdir.z;

  if ((tmin > tzmax) || (tzmin > tmax))
    return false;
  if (tzmin > tmin)
    tmin = tzmin;
  if (tzmax < tmax)
    tmax = tzmax;

  return true;
}

std::experimental::optional<BoxIntersection>
Box::intersects(const Box &other) const {
  static const std::array<glm::vec3, 6> faces = {
      glm::vec3(-1, 0, 0), // 'left' face normal (-x direction)
      glm::vec3(1, 0, 0),  // 'right' face normal (+x direction)
      glm::vec3(0, -1, 0), // 'far' face normal (-y direction)
      glm::vec3(0, 1, 0),  // 'near' face normal (+y direction)
      glm::vec3(0, 0, -1), // 'bottom' face normal (-z direction)
      glm::vec3(0, 0, 1),  // 'top' face normal (+z direction)
  };

  glm::vec3 maxa = this->max();
  glm::vec3 mina = this->min();

  // TODO: This swapping is kind of nasty. Look throgh the whol BB thing.
  if (maxa.x < mina.x) {
    std::swap(maxa.x, mina.x);
  }
  if (maxa.y < mina.y) {
    std::swap(maxa.y, mina.y);
  }
  if (maxa.z < mina.z) {
    std::swap(maxa.z, mina.z);
  }

  glm::vec3 maxb = other.max();
  glm::vec3 minb = other.min();

  if (maxb.x < minb.x) {
    std::swap(maxb.x, minb.x);
  }
  if (maxb.y < minb.y) {
    std::swap(maxb.y, minb.y);
  }
  if (maxb.z < minb.z) {
    std::swap(maxb.z, minb.z);
  }

  std::array<float, 6> distances = {
      (maxb.x - mina.x), // distance of box 'b' to face on 'left' side of 'a'.
      (maxa.x - minb.x), // distance of box 'b' to face on 'right' side of 'a'.
      (maxb.y - mina.y), // distance of box 'b' to face on 'bottom' side of 'a'.
      (maxa.y - minb.y), // distance of box 'b' to face on 'top' side of 'a'.
      (maxb.z - mina.z), // distance of box 'b' to face on 'far' side of 'a'.
      (maxa.z - minb.z), // distance of box 'b' to face on 'near' side of 'a'.
  };

  glm::vec3 normal(0.0f);
  float distance = 0.0f;
  for (int i = 0; i < 6; i++) {
    // box does not intersect face. So boxes don't intersect at all.
    if (distances[i] < 0.0f) {
      return std::experimental::optional<BoxIntersection>();
    }
    // face of least intersection depth. That's our candidate.
    if ((i == 0) || (distances[i] < distance)) {
      // fcoll = i;
      normal = faces[i];
      distance = distances[i];
    }
  }
  return BoxIntersection(normal, distance);
}

glm::vec3 Box::intersects_simple(const Box &other) {
  auto intersection = intersects(other);
  if (intersection) {
    return intersection->normal * intersection->distance;
  } else {
    return glm::vec3(0.0f);
  }
}

void Box::transform(const glm::mat4 &transform) {
  position.x = transform[3][0];
  position.y = transform[3][1];
  position.z = transform[3][2];
}

float Box::volume() const { return glm::abs(glm::compMul(max() - min())); }

unsigned int Box::id() const {
  return id_;
}

glm::vec3 Box::size() const {
  return glm::vec3(glm::abs(max().x - min().x), glm::abs(max().y - min().y),
                   glm::abs(max().z - min().z));
}
}
