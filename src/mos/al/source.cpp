#include <mos/al/source.hpp>

namespace mos::al {

void Source::update(const mos::apu::Source &source)
{
  alSourcei(id, AL_LOOPING, static_cast<ALint>(source.loop));
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

Source::Source(const apu::Source &source) : Resource(alGenSources, alDeleteSources) {
  alGenSources(1, &id);
  update(source);
}
}
