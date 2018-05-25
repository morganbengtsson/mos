#pragma once

#include <initializer_list>
#include <vector>
#include <mos/aud/buffer_source.hpp>
#include <mos/aud/listener.hpp>
#include <mos/aud/stream_source.hpp>
#include <mos/aud/buffer_sources.hpp>
#include <mos/aud/stream_sources.hpp>

namespace mos {
namespace aud {

/** Full scene for audio processing. */
class Scene final {
public:
  Scene(const BufferSources &buffer_sources,
        const StreamSources &stream_sources,
        const Listener &listener);
  Scene();

  const BufferSources buffer_sources;
  const StreamSources stream_sources;
  const Listener listener;
};
}
}