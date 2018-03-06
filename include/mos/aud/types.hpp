#pragma once
#include <memory>
#include <mos/aud/buffer.hpp>
#include <mos/aud/stream.hpp>

namespace mos {
namespace aud {
using SharedBuffer = std::shared_ptr<Buffer>;
using SharedAudioStream = std::shared_ptr<Stream>;
}
}

