#ifndef MOS_AUDIO_BATCH_HPP
#define MOS_AUDIO_BATCH_HPP

#include <initializer_list>
#include <mos/audio/audio_buffer_source.hpp>
#include <mos/audio/audio_listener.hpp>
#include <mos/audio/audio_stream_source.hpp>
#include <vector>

namespace mos {
class AudioBatch {
public:
  using AudioBufferSources = std::vector<AudioBufferSource>;
  using AudioStreamSources = std::vector<AudioStreamSource>;
  template <class Tb, class Ts>
  AudioBatch(const Tb buffers_begin, const Tb buffers_end,
             const Ts streams_begin, const Ts streams_end,
             const AudioListener &listener)
      : buffer_sources(buffers_begin, buffers_end),
        stream_sources(streams_begin, streams_end), listener(listener) {}
  AudioBatch(const std::initializer_list<AudioBufferSource> buffer_sources,
             const std::initializer_list<AudioStreamSource> stream_sources,
             const AudioListener &listener);
  AudioBufferSources buffer_sources;
  AudioStreamSources stream_sources;
  AudioListener listener;
};
}

#endif // MOS_AUDIO_BATCH_HPP
