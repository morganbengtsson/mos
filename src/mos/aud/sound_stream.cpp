#include <mos/aud/sound_streams.hpp>

namespace mos::aud {

Sound_stream::Sound_stream(Shared_stream stream,
                           Source source)
    : stream(std::move(stream)), source(source) {}
} // namespace mos
