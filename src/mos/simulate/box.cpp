#include <mos/simulate/box.hpp>
#include <glm/gtx/component_wise.hpp>
#include <mos/gfx/model.hpp>

namespace mos {

Box::Box(const glm::vec3 &extent, const glm::vec3 &position)
    : extent(extent), position(position){}

Box::Box() {}

Box Box::create_from_model(const gfx::Model &model, const glm::mat4 &transform) {
  std::vector<gfx::Vertex> all_vertices;

  std::function<void(const gfx::Model &, const glm::mat4 &, std::vector<gfx::Vertex> &)>
      create = [&](const gfx::Model &m, const glm::mat4 &t, std::vector<gfx::Vertex> &v) {
    if (m.mesh != nullptr) {
      for (auto &v0 : m.mesh->vertices) {
        auto vt0 = v0;
        vt0.position = glm::vec3(t * m.transform * glm::vec4(v0.position, 1.0f));
        v.push_back(vt0);
      }
    }
    for (auto &child : m.models) {
      create(child, t * m.transform, v);
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

void Box::transform(const glm::mat4 &transform) {
  position.x = transform[3][0];
  position.y = transform[3][1];
  position.z = transform[3][2];
}

float Box::volume() const { return glm::abs(glm::compMul(max() - min())); }

glm::vec3 Box::size() const {
  return glm::vec3(glm::abs(max().x - min().x), glm::abs(max().y - min().y),
                   glm::abs(max().z - min().z));
}
std::ostream &operator<<(std::ostream &os, const Box &box) {
  os << "Box object " << " extent: " << box.extent << " position: " << box.position << std::endl;
  return os;
}


}
