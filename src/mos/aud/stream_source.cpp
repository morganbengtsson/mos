#include <mos/aud/stream_source.hpp>

namespace mos {
namespace aud {

Stream_source::Stream_source(const SharedStream &stream,
                                     const Source &source)
    : stream(stream), source(source) {}

Stream_source::~Stream_source() {}
}
}
