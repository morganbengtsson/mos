#include "streamsource.hpp"

namespace mo {

StreamSource::StreamSource(std::shared_ptr<mo::Stream> stream,
                           const glm::vec3 position,
                           const glm::vec3 velocity,
                           const float gain,
                           const float pitch):
    Source(position, velocity, pitch, gain),
    stream(stream) {

}

StreamSource::~StreamSource()
{

}

}
