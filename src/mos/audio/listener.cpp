#include <mos/audio/audio_listener.hpp>

mos::AudioListener::AudioListener(const glm::vec3 &position, const glm::vec3 &direction,
                        const glm::vec3 &up, const glm::vec3 &velocity,
                        const float gain)
    : position(position), direction(direction), up(up), velocity(velocity), gain(gain){}

mos::AudioListener::~AudioListener() {}
