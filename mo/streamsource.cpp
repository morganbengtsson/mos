#include "streamsource.hpp"

namespace mo {

StreamSource::StreamSource(std::shared_ptr<mo::Stream> stream):
    stream(stream) {

}

StreamSource::~StreamSource()
{

}

}
