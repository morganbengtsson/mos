#include <mos/aud/stream_source.hpp>

namespace mos {
namespace aud {

Stream_source::Stream_source(Shared_stream stream,
                             Source source)
    : stream(std::move(stream)), source(source) {}

}
}
