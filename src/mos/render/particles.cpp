#include <mos/render/particles.hpp>
#include <algorithm>

namespace mos {

//TODO: Why 10000?
//TODO: Inherit from abstract mesh?
std::atomic_uint Particles::current_id_(10000);

Particles::Particles() : id_(current_id_++) {
  invalidate();
}

Particles::~Particles() {}

unsigned int Particles::id() const { return id_; }

void Particles::invalidate() {
  modified_ = std::chrono::system_clock::now();
}

void Particles::add(const Particle particle) {
  particles_.push_back(particle);
  invalidate();
}

Particle Particles::back() { return particles_.back(); }

const Particle *Particles::data() const { return particles_.data(); }

unsigned int Particles::size() const { return particles_.size(); }

void Particles::clear() { particles_.clear(); }

void Particles::sort(const glm::vec3 &position) {
  std::sort(particles_.begin(), particles_.end(),
            [&](const mos::Particle &a, const mos::Particle &b) -> bool {
              auto a_distance1 = glm::distance(a.position, position);
              auto b_distance1 = glm::distance(b.position, position);
              return a_distance1 > b_distance1;
            });
}

Particle &Particles::operator[](size_t index) {
  return particles_[index];
}
const Particle &Particles::operator[](size_t index) const {
  return particles_[index];
}

}
