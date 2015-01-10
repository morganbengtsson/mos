#ifndef STREAMSOURCE_HPP
#define STREAMSOURCE_HPP

#include <glm/glm.hpp>
#include <memory>

#include "stream.hpp"
#include "source.hpp"

namespace mo {

class StreamSource : public Source
{
public:
    StreamSource(std::shared_ptr<Stream> stream);
    virtual ~StreamSource();
    std::shared_ptr<Stream> stream;
};

}

#endif // STREAMSOURCE_HPP
