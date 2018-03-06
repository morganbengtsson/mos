#include <mos/aud/scene.hpp>
namespace mos {
AudioScene::AudioScene(
    const std::initializer_list<mos::AudioBufferSource> buffer_sources,
    const std::initializer_list<mos::StreamSource> stream_sources,
    const AudioListener &listener)
    : AudioScene(buffer_sources.begin(), buffer_sources.end(),
                 stream_sources.begin(), stream_sources.end(), listener) {}

AudioScene::AudioScene() {
}
}
