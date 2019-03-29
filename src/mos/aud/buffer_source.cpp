#include <mos/aud/buffer_source.hpp>

namespace mos {
namespace aud {

Buffer_source::Buffer_source(Shared_buffer buffer,
                             Source source)
    : buffer(std::move(buffer)), source(source), time_(.0f) {}

void Buffer_source::input(const float dt) {
  if (source.playing) {
    time_ += dt;
  }
  if (time_ > (buffer->duration() / source.pitch) && !source.loop) {
    source.playing = false;
    time_ = .0f;
  }
}
}
}
