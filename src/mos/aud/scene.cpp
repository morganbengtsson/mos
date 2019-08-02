#include <mos/aud/scene.hpp>
namespace mos {
namespace aud {
Scene::Scene(Speakers buffer_sources, Stream_speakers stream_sources,
             Listener listener)
    : speakers(std::move(buffer_sources)),
      stream_speakers(std::move(stream_sources)), listener(listener) {}
}
}
