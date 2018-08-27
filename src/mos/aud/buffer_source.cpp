#include <mos/aud/buffer_source.hpp>

namespace mos {
namespace aud {

BufferSource::BufferSource(const SharedBuffer &buffer,
                                     const Source &source)
    : buffer(buffer), source(source), time_(.0f) {}

BufferSource::~BufferSource() {}

void BufferSource::input(const float dt) {
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