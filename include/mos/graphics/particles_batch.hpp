#ifndef MOS_PARTICLESBATCH_HPP
#define MOS_PARTICLESBATCH_HPP

#include <mos/graphics/particles.hpp>

namespace mos {

class ParticlesBatch {
public:
  ParticlesBatch(const Particles &particles, const glm::mat4 &view,
                 const glm::mat4 &projection);
  Particles particles;
  glm::mat4 view;
  glm::mat4 projection;
};
}

#endif // MOS_PARTICLESBATCH_HPP
