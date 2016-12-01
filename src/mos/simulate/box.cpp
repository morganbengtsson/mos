#include <mos/simulate/box.hpp>
#include <glm/gtx/component_wise.hpp>
#include <mos/render/model.hpp>

namespace mos {

std::atomic_uint Box::current_id_;

Box::Box(const glm::vec3 &extent, const glm::vec3 &position)
    : extent(extent), position(position), id_(current_id_++) {}

Box::Box() {}

Box Box::create_from_model(const Model &model, const glm::mat4 &transform) {
  std::vector<mos::Vertex> all_vertices;
  std::function<void(const Model &, const glm::mat4 &, std::vector<Vertex> &)>
      create = [&](const Model &m, const glm::mat4 &t, std::vector<Vertex> &v) {
    if (m.mesh) {
      for (auto &v0 : m.mesh->vertices()) {
        auto vt0 = v0;
        vt0.position = glm::vec3(t * m.transform * glm::vec4(v0.position, 1.0f));
        v.push_back(vt0);
      }
    }
    for (auto &child : m.models) {
      create(child, m.transform, v);
    }
  };
  create(model, transform, all_vertices);
  return mos::Box(all_vertices.begin(), all_vertices.end(), glm::mat4(1.0f));
}

Box Box::create_from_min_max(const glm::vec3 &min, const glm::vec3 &max) {
  auto extent = (max - min) / 2.0f;
  auto position = min + extent;
  return Box(extent, position);
}

glm::vec3 Box::min() const { return position - extent; }
glm::vec3 Box::max() const { return position + extent; }

bool Box::intersects(const glm::vec3 &origin, const glm::vec3 &direction) const {
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

bool Box::intersects(const Ray &ray) const {
  return intersects(ray.origin, ray.direction());
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

Box::OptionalIntersection Box::intersection(const Box &other) const {
  auto intersection = intersects(other);
  if (intersection) {
    return intersection->normal * intersection->distance;
  } else {
    return OptionalIntersection();
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
