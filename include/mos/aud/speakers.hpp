#pragma once

#include <initializer_list>
#include <mos/aud/speaker.hpp>
#include <mos/core/container.hpp>

namespace mos {
namespace aud {

/** Container of buffer sources. */
using Speakers = Container<Speaker>;

}
}
