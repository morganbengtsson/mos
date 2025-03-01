#include <mos/aud/listener.hpp>
namespace mos::aud {

Listener::Listener(const glm::vec3 &position, const glm::vec3 &direction,
                                  const glm::vec3 &up, const glm::vec3 &velocity,
                                  const float gain)
    : position(position), direction(direction), up(up), velocity(velocity), gain(gain) {}

} // namespace mos::aud
