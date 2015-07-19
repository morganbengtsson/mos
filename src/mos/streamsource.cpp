#include "streamsource.hpp"

namespace mos {

StreamSource::StreamSource(std::shared_ptr<mos::Stream> stream,
                           const glm::vec3 position,
                           const glm::vec3 velocity,
                           const float gain,
                           const float pitch,
                           const bool loop,
                           const bool playing,
                           const bool occluded):
    Source(position, velocity, pitch, gain, loop, playing, occluded),
    stream(stream) {

}

StreamSource::~StreamSource()
{

}

}
