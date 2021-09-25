#include <mos/al/filter.hpp>
#include <mos/al/source.hpp>
#include <glm/glm.hpp>

namespace mos::al {

Filter::Filter(Filter &&filter) noexcept : id(filter.id) {
  filter.id = 0;
}

Filter &Filter::operator=(Filter &&filter) noexcept {
  if (this != &filter) {
    release();
    std::swap(id, filter.id);
  }
  return *this;
}

Filter::~Filter() {
  release();
}

void Filter::update(const aud::Source &source, const float dt) {
  ALfloat al_gain{0.0f};
  alGetFilterf(id, AL_LOWPASS_GAIN, &al_gain);
  auto error = glm::clamp((1.0f - source.obstructed), 0.0f, 1.0f) - al_gain;
  float gain = al_gain + error * dt;

  ALfloat al_gain_hf{0.0f};
  alGetFilterf(id, AL_LOWPASS_GAINHF, &al_gain_hf);
  auto error_hf = glm::clamp((1.0f - source.obstructed), 0.0f, 1.0f) - al_gain_hf;
  float gain_hf = al_gain_hf + error_hf * dt;

  alFilteri(id, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
  alFilterf(id, AL_LOWPASS_GAIN, gain);      // 0.5f
  alFilterf(id, AL_LOWPASS_GAINHF, gain_hf); // 0.01f
}

Filter::Filter(const Source& source) {
  alGenFilters(1, &id);
  alFilteri(id, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
  alSourcei(source.id, AL_DIRECT_FILTER, id);
}

void Filter::release() {
  alDeleteFilters(1, &id);
  id = 0;
}
}
