#include <mos/gfx/particles.hpp>
#include <algorithm>

namespace mos {
namespace gfx {

//TODO: Why 10000?
//TODO: Inherit from abstract mesh?
std::atomic_uint Particles::current_id_(10000);

Particles::Particles() : id_(current_id_++) {
}

Particles::~Particles() {}

unsigned int Particles::id() const { return id_; }

void Particles::sort(const glm::vec3 &position) {
  std::sort(particles.begin(), particles.end(),
            [&](const Particle &a, const Particle &b) -> bool {
              auto a_distance1 = glm::distance(a.position, position);
              auto b_distance1 = glm::distance(b.position, position);
              return a_distance1 > b_distance1;
            });
}
}
}
