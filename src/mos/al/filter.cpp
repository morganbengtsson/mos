#include <mos/al/filter.hpp>
#include <mos/al/source.hpp>
#include <glm/glm.hpp>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>

namespace mos::al {

void Filter::update(const apu::Source &source, const float dt) {
  ALfloat al_gain{0.0F};
  alGetFilterf(id, AL_LOWPASS_GAIN, &al_gain);
  auto error = glm::clamp((1.0F - source.obstructed), 0.0F, 1.0F) - al_gain;
  float gain = al_gain + (error * dt);

  ALfloat al_gain_hf{0.0F};
  alGetFilterf(id, AL_LOWPASS_GAINHF, &al_gain_hf);
  auto error_hf = glm::clamp((1.0F - source.obstructed), 0.0F, 1.0F) - al_gain_hf;
  float gain_hf = al_gain_hf + (error_hf * dt);

  alFilteri(id, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
  alFilterf(id, AL_LOWPASS_GAIN, gain);      // 0.5f
  alFilterf(id, AL_LOWPASS_GAINHF, gain_hf); // 0.01f
}

Filter::Filter(const Source& source) : Resource(alGenFilters, alDeleteFilters){
  alGenFilters(1, &id);
  alFilteri(id, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
  alSourcei(source.id, AL_DIRECT_FILTER, id);
}
} // namespace mos::al
