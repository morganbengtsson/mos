#include <mos/aud/stream_source.hpp>

namespace mos {
namespace aud {

StreamSource::StreamSource(const SharedStream &stream,
                                     const Source &source)
    : stream(stream), source(source) {}

StreamSource::~StreamSource() {}
}
}
