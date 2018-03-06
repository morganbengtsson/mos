#include <mos/aud/scene.hpp>
namespace mos {
namespace aud {
Scene::Scene(
    const std::initializer_list<BufferSource> buffer_sources,
    const std::initializer_list<StreamSource> stream_sources,
    const Listener &listener)
    : Scene(buffer_sources.begin(), buffer_sources.end(),
                 stream_sources.begin(), stream_sources.end(), listener) {}

Scene::Scene() {
}
}
}