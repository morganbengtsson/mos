#ifndef MOS_STREAMSOURCE_HPP
#define MOS_STREAMSOURCE_HPP

#include <glm/glm.hpp>
#include <memory>

#include "stream.hpp"
#include "source.hpp"

namespace mos {

/**
 * @brief The StreamSource class, for audio streaming
 *
 * Stream audio from an *ogg audio resource. Used with the Audio class for playback.
 */
class StreamSource
{
public:

    /**
     * @brief StreamSource constructor.
     * @param stream
     * @param source
     */
    explicit StreamSource(const std::shared_ptr<Stream> & stream,
                          const Source & source = Source());
    virtual ~StreamSource();

    /**
     * @brief Stream used for the source.
     * @todo Should it be shared?
     */
    std::shared_ptr<Stream> stream;
    Source source;
};

}

#endif // MOS_STREAMSOURCE_HPP
