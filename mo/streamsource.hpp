#ifndef STREAMSOURCE_HPP
#define STREAMSOURCE_HPP

#include <glm/glm.hpp>
#include <memory>

#include "stream.hpp"

namespace mo {

class StreamSource
{
public:
    StreamSource(std::shared_ptr<Stream> stream,
                 const glm::vec3 position = glm::vec3(0.0f));
    glm::vec3 position;
    glm::vec3 velocity;
    float pitch;
    float gain;
    bool loop;
    bool playing;
    std::shared_ptr<Stream> stream;
    unsigned int id() const;
private:
    unsigned int id_;
};

}

#endif // STREAMSOURCE_HPP
