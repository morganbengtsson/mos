#include <mos/audio/listener.hpp>

mos::Listener::Listener(const glm::vec3 &position, const glm::vec3 &direction,
                        const glm::vec3 &up, const glm::vec3 &velocity,
                        const float gain)
    : position(position), direction(direction), up(up), velocity(velocity), gain(gain){}

mos::Listener::~Listener() {}
