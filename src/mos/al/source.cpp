#include <mos/al/source.hpp>

namespace mos::al {

Source::Source(Source &&source) noexcept : id(source.id) {
  source.id = 0;
}

Source &Source::operator=(Source &&source) noexcept {
  if (this != &source) {
    release();
    std::swap(id, source.id);
  }
  return *this;
}

Source::~Source() {
  release();
}

void Source::update(const mos::aud::Source &source)
{
  alSourcei(id, AL_LOOPING, source.loop);
  alSourcef(id, AL_PITCH, source.pitch);
  //alSourcef(id, AL_GAIN, buffer_source.source.gain);
  alSource3f(id, AL_POSITION, source.position.x,
             source.position.y, source.position.z);
  alSource3f(id, AL_VELOCITY, source.velocity.x,
             source.velocity.y, source.velocity.z);
}

void Source::stop()
{
  alSourceStop(id);
}

Source::Source(const aud::Source &source) {
  alGenSources(1, &id);
  update(source);
}

void Source::release() {
  alDeleteSources(1, &id);
  id = 0;
}
}
