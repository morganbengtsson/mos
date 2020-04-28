#include <mos/gfx/cloud.hpp>
#include <algorithm>

namespace mos {
namespace gfx {

Cloud::Cloud(Shared_texture_2D emission_map,
                              Points points) :
  texture(std::move(emission_map)),
  points(std::move(points)) {}

void Cloud::sort(const glm::vec3 &position) {
  std::sort(points.begin(), points.end(),
            [&](const Point &a, const Point &b) -> bool {
              auto a_distance1 = glm::distance(a.position, position);
              auto b_distance1 = glm::distance(b.position, position);
              return a_distance1 > b_distance1;
            });
}

}
}
