#include <mos/aud/stream_source.hpp>

namespace mos {
namespace aud {

Stream_source::Stream_source(const Shared_stream &stream,
                                     const Source &source)
    : stream(stream), source(source) {}

Stream_source::~Stream_source() {}
}
}
