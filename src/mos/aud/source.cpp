#include <mos/aud/source.hpp>

namespace mos::aud {

std::atomic_uint Source::current_id_;

Source::Source(const glm::vec3 &position,
               const glm::vec3 &velocity,
                         const float pitch, const float gain, const bool loop,
                         const bool playing, const float obstructed)
    : position(position), velocity(velocity), pitch(pitch), gain(gain),
      loop(loop), playing(playing), obstructed(obstructed), id_(current_id_++) {
}

auto Source::id() const -> unsigned int { return id_; }
} // namespace mos::aud
