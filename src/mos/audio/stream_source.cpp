#include <mos/audio/audio_stream_source.hpp>

namespace mos {

AudioStreamSource::AudioStreamSource(const std::shared_ptr<AudioStream> &stream,
                           const AudioSource &source)
    : stream(stream), source(source) {}

AudioStreamSource::~AudioStreamSource() {}
}
