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

/** Audio scene with listener and audio sources. */
class Scene final {
public:
  Scene(Buffer_sources buffer_sources,
        Stream_sources stream_sources,
        Listener listener);
  Scene();

  Buffer_sources buffer_sources;
  Stream_sources stream_sources;
  Listener listener;
};
}
}
