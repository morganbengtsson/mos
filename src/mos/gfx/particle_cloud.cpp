#include <mos/gfx/particle_cloud.hpp>
#include <algorithm>

namespace mos {
namespace gfx {

Particle_cloud::Particle_cloud() {
}

Particle_cloud::Particle_cloud(const Shared_texture_2D& emission_map, const Particles & particles) :
emission_map(emission_map), particles(particles) {}

void Particle_cloud::sort(const glm::vec3 &position) {
  std::sort(particles.begin(), particles.end(),
            [&](const Particle &a, const Particle &b) -> bool {
              auto a_distance1 = glm::distance(a.position, position);
              auto b_distance1 = glm::distance(b.position, position);
              return a_distance1 > b_distance1;
            });
}

}
}
