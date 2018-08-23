#pragma once

#include <mos/core/container.hpp>
#include <mos/aud/stream_source.hpp>

namespace mos {
namespace aud {

/** Collection of stream sources. */
using StreamSources = Container<StreamSource>;

}
}