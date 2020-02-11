#include <mos/gfx/particle_cloud.hpp>
#include <algorithm>

namespace mos {
namespace gfx {

Particle_cloud::Particle_cloud(Shared_texture_2D emission_map,
                               Particles particles) :
  texture(std::move(emission_map)),
  particles(std::move(particles)) {}

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
