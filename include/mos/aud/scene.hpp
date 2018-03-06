#pragma once

#include <initializer_list>
#include <vector>
#include <mos/aud/buffer_source.hpp>
#include <mos/aud/listener.hpp>
#include <mos/aud/stream_source.hpp>

namespace mos {
namespace aud {
/** Full scene for audio processing. */
class Scene final {
public:
  using BufferSources = std::vector<BufferSource>;
  using StreamSources = std::vector<StreamSource>;

  template<class Tb, class Ts>
  Scene(const Tb buffers_begin, const Tb buffers_end,
             const Ts streams_begin, const Ts streams_end,
             const Listener &listener)
      : buffer_sources(buffers_begin, buffers_end),
        stream_sources(streams_begin, streams_end), listener(listener) {}

  Scene(const std::initializer_list<BufferSource> buffer_sources,
             const std::initializer_list<StreamSource> stream_sources,
             const Listener &listener);

  Scene();

  const BufferSources buffer_sources;
  const StreamSources stream_sources;
  const Listener listener;
};
}
}