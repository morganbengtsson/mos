#include <mos/aud/scene.hpp>
namespace mos {
namespace aud {
Scene::Scene(
    Buffer_sources buffer_sources,
    Stream_sources stream_sources,
    Listener listener)
    : buffer_sources(std::move(buffer_sources)),
      stream_sources(std::move(stream_sources)),
      listener(listener) {}
}
}
