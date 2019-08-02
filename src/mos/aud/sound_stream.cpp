#include <mos/aud/sound_streams.hpp>

namespace mos {
namespace aud {

Stream_speaker::Stream_speaker(Shared_stream stream,
                             Source source)
    : stream(std::move(stream)), source(source) {}

} // namespace aud
} // namespace mos
