#include "streamsource.hpp"

namespace mo {

StreamSource::StreamSource(std::shared_ptr<mo::Stream> stream,
                           const glm::vec3 position) :
    stream(stream),
    position(position),
    velocity(glm::vec3(0.0f)),
    gain(1.0f),
    pitch(1.0f),
    loop(false),
    playing(true) {
        static unsigned int current_id = 0;
        id_ = current_id++;
}

unsigned int StreamSource::id() const
{
    return id_;
}
}
