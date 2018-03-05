#ifndef AUDIO_TYPES_HPP
#define AUDIO_TYPES_HPP

#include <memory>
#include <mos/audio/audio_buffer.hpp>
#include <mos/audio/audio_stream.hpp>

namespace mos {
    using SharedAudioBuffer = std::shared_ptr<AudioBuffer>;
    using SharedAudioStream = std::shared_ptr<AudioStream>;
}

#endif // AUDIO_TYPES_HPP

