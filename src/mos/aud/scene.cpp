#include <mos/aud/scene.hpp>
namespace mos {
namespace aud {
Scene::Scene(Sounds buffer_sources, Sound_streams stream_sources,
             Listener listener)
    : sounds(std::move(buffer_sources)),
      sound_streams(std::move(stream_sources)), listener(listener) {}
}
}
