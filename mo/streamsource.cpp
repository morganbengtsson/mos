#include "streamsource.hpp"

namespace mo {

StreamSource::StreamSource(std::shared_ptr<mo::Stream> stream, const glm::vec3 position):
    Source(position),
    stream(stream) {

}

StreamSource::~StreamSource()
{

}

}
