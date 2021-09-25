#include <mos/al/buffer.hpp>

namespace mos::al {

Buffer::Buffer(Buffer &&buffer) noexcept : id(buffer.id) {
  buffer.id = 0;
}

Buffer &Buffer::operator=(Buffer &&buffer) noexcept {
  if (this != &buffer) {
    release();
    std::swap(id, buffer.id);
  }
  return *this;
}

Buffer::~Buffer() {
  release();
}

Buffer::Buffer(const aud::Buffer &buffer) {
  alGenBuffers(1, &id);
  auto format =
      buffer.channels() == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
  long data_size = std::distance(buffer.begin(), buffer.end());
  const ALvoid *data = buffer.data();
  alBufferData(id, format, data, data_size * sizeof(short),
               buffer.sample_rate());
}

void Buffer::release() {
  alDeleteBuffers(1, &id);
  id = 0;
}
}
