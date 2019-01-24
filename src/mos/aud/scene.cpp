#include <mos/aud/scene.hpp>
namespace mos {
namespace aud {
Scene::Scene(
    const Buffer_sources &buffer_sources,
    const Stream_sources &stream_sources,
    const Listener &listener)
    : buffer_sources(std::move(buffer_sources)),
      stream_sources(std::move(stream_sources)),
      listener(std::move(listener)) {}
}
}
