#include <mos/al/buffer.hpp>

namespace mos::al {

Buffer::Buffer(const aud::Buffer &buffer) : resource(alGenBuffers, alDeleteBuffers) {
  auto format =
      buffer.channels() == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
  long data_size = std::distance(buffer.begin(), buffer.end());
  const ALvoid *data = buffer.data();
  alBufferData(resource.id, format, data, data_size * sizeof(short),
               buffer.sample_rate());
}

}
