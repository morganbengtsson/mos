#include "audio/listener.hpp"

mos::Listener::Listener(const glm::vec3 &position,                        
                        const glm::vec3 &velocity,
                        const glm::vec3 &orientation):
    position(position),
    orientation(orientation),
    velocity(velocity){
}

mos::Listener::~Listener() {
}


