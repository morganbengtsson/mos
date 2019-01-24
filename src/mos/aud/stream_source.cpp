#include <mos/aud/stream_source.hpp>

namespace mos {
namespace aud {

Stream_source::Stream_source(const Shared_stream &stream,
                                     const Source &source)
    : stream(std::move(stream)), source(std::move(source)) {}

}
}
