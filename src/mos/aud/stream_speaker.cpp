#include <mos/aud/stream_source.hpp>

namespace mos {
namespace aud {

Stream_speaker::Stream_speaker(Shared_stream stream,
                             Source source)
    : stream(std::move(stream)), source(source) {}

}
}
