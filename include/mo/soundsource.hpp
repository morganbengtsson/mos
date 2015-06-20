#ifndef SOUNDSOURCE_HPP
#define SOUNDSOURCE_HPP

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "sound.hpp"
#include "source.hpp"

namespace mo {

/*!
 * A classed used for audio playback. Contains sounds, together with a position.
 * That is used if 3D audio is enabled.
 */
class SoundSource : public Source {
public:

    /*!
     * @brief SoundSource constructor
     * @param sound shared pointer
     * @param position
     */
    explicit SoundSource(const std::shared_ptr<Sound> sound,
                const glm::vec3 position = glm::vec3(0.0f),
                const glm::vec3 velocity = glm::vec3(0.0f),
                const float gain = 1.0f,
                const float pitch = 1.0f,
                const bool loop = false,
                const bool playing = false,
                const bool occluded = false);
    virtual ~SoundSource();
    std::shared_ptr<Sound> sound;
};
}

#endif // SOUNDSOURCE_HPP
