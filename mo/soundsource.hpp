#ifndef SOUNDSOURCE_HPP
#define SOUNDSOURCE_HPP

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "sound.hpp"
#include "stream.hpp"

namespace mo {

/*!
 * A classed used for audio playback. Contains sounds, together with a position.
 * That is used if 3D audio is enabled.
 */
class SoundSource {
public:

    SoundSource(const std::shared_ptr<Sound> sound,
           const glm::vec3 position = glm::vec3(0.0f),
           const bool loop = false);
    virtual ~SoundSource();
    glm::vec3 position;
    bool loop;
    std::shared_ptr<Sound> sound;
    unsigned int id() const;
private:
    unsigned int id_;


};
}

#endif // SOUNDSOURCE_HPP
