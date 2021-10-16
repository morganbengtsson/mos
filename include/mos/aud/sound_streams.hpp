#pragma once

#include <mos/aud/sound_stream.hpp>
#include <mos/core/container.hpp>

namespace mos::aud {

/** Collection of stream sources. */
using Sound_streams = Container<Sound_stream>;

} // namespace mos::aud
