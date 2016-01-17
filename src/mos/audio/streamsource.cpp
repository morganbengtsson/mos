#include <mos/audio/streamsource.hpp>

namespace mos {

StreamSource::StreamSource(const std::shared_ptr<mos::Stream> & stream,
                           const Source & source):
    stream(stream), source(source) {
}

StreamSource::~StreamSource() {
}

}
