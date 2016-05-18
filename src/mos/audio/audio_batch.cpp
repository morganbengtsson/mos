#include <mos/audio/audio_batch.hpp>
namespace mos {

AudioBatch::AudioBatch(
    const std::initializer_list<mos::AudioBufferSource> buffer_sources,
    const std::initializer_list<mos::AudioStreamSource> stream_sources,
    const AudioListener &listener)
    : AudioBatch(buffer_sources.begin(), buffer_sources.end(),
                 stream_sources.begin(), stream_sources.end(), listener) {}
}
