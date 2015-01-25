#ifndef STREAMSOURCE_HPP
#define STREAMSOURCE_HPP

#include <glm/glm.hpp>
#include <memory>

#include "stream.hpp"
#include "source.hpp"

namespace mo {

/*!
 * \brief The StreamSource class, for audio streaming
 *
 * Stream audio from an *ogg audio resource. Used with the Audio class for playback.
 */
class StreamSource : public Source
{
public:
    /*!
     * \brief StreamSource constructor
     * \param ogg stream
     */
    StreamSource(std::shared_ptr<Stream> stream, const glm::vec3 position = glm::vec3(0.0f));
    virtual ~StreamSource();
    std::shared_ptr<Stream> stream;
};

}

#endif // STREAMSOURCE_HPP
