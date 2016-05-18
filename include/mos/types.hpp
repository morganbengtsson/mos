#ifndef TYPES_HPP
#define TYPES_HPP

#include <memory>
#include <mos/audio/audio_buffer.hpp>
#include <mos/audio/audio_stream.hpp>
#include <mos/graphics/mesh.hpp>

namespace mos {
using SharedAudioBuffer = std::shared_ptr<AudioBuffer>;
using SharedMesh = std::shared_ptr<Mesh>;
using SharedAudioStream = std::shared_ptr<AudioStream>;
}

#endif // TYPES_HPP
