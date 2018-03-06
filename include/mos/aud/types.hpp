#pragma once
#include <memory>

namespace mos {
namespace aud {
class Buffer;
class Stream;
using SharedBuffer = std::shared_ptr<Buffer>;
using SharedStream = std::shared_ptr<Stream>;
}
}

