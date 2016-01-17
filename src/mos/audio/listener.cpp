#include <mos/audio/listener.hpp>

mos::Listener::Listener(const glm::vec3 &position, const glm::vec3 &orientation,
                        const glm::vec3 &velocity)
    : position(position), orientation(orientation), velocity(velocity) {}

mos::Listener::~Listener() {}
