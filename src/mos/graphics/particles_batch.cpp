#include <mos/graphics/particles_batch.hpp>

namespace mos {
mos::ParticlesBatch::ParticlesBatch(const Particles &particles,
                                    const glm::mat4 &view,
                                    const glm::mat4 &projection)
    : particles(particles), view(view), projection(projection){}
}
