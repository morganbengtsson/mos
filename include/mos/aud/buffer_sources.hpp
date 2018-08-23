#pragma once

#include <initializer_list>
#include <mos/aud/buffer_source.hpp>
#include <mos/core/container.hpp>

namespace mos {
namespace aud {

/** Container of buffer sources. */
using BufferSources = Container<BufferSource>;

}
}