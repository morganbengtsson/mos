#ifndef STREAMSOURCE_HPP
#define STREAMSOURCE_HPP

#include <glm/glm.hpp>
#include <memory>

#include "stream.hpp"
#include "source.hpp"

namespace mo {

/*!
 * @brief The StreamSource class, for audio streaming
 *
 * Stream audio from an *ogg audio resource. Used with the Audio class for playback.
 */
class StreamSource : public Source
{
public:
    /*!
     * @brief StreamSource constructor
     * @param stream, position, velocity, gain, pitch, loop, playing
     */
    explicit StreamSource(std::shared_ptr<Stream> stream,
                 const glm::vec3 position = glm::vec3(0.0f),
                 const glm::vec3 velocity = glm::vec3(0.0f),
                 const float gain = 1.0f,
                 const float pitch = 1.0f,
                 const bool loop = false,
                 const bool playing = false,
                 const bool occluded = false);
    virtual ~StreamSource();

    std::shared_ptr<Stream> stream;
};

}

#endif // STREAMSOURCE_HPP
