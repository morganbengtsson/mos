#include <mos/aud/scene.hpp>
namespace mos {
namespace aud {
Scene::Scene(
    Speakers buffer_sources,
    Stream_speakers stream_sources,
    Listener listener)
    : buffer_sources(std::move(buffer_sources)),
      stream_sources(std::move(stream_sources)),
      listener(listener) {}
}
}
