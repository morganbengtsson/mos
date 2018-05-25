#include <mos/aud/scene.hpp>
namespace mos {
namespace aud {
Scene::Scene(
    const BufferSources &buffer_sources,
    const StreamSources &stream_sources,
    const Listener &listener)
    : buffer_sources(buffer_sources),
      stream_sources(stream_sources),
      listener(listener) {}

Scene::Scene() {
}
}
}