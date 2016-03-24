#ifndef TYPES_HPP
#define TYPES_HPP

#include <vector>
#include <mos/audio/soundsource.hpp>
#include <mos/audio/streamsource.hpp>
#include <mos/graphics/model.hpp>

namespace mos {
  using SoundSources = std::vector<SoundSource>;
  using StreamSources = std::vector<StreamSource>;
  using Models = std::vector<Model>;
}


#endif // TYPES_HPP

