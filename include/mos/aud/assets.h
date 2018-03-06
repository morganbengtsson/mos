#pragma once
#include <map>
#include <memory>
#include <unordered_map>
#include <mos/audio/audio_buffer.hpp>
#include <mos/audio/audio_stream.hpp>

namespace mos {
namespace aud {

class Assets final {
public:
  using AudioBufferMap =
  std::unordered_map<std::string, std::shared_ptr<AudioBuffer>>;
  using AudioBufferPair = std::pair<std::string, std::shared_ptr<AudioBuffer>>;

  using SharedAudioBuffer = std::shared_ptr<mos::AudioBuffer>;
  using SharedAudioStream = std::shared_ptr<mos::AudioStream>;

  Assets(const std::string directory = "assets/");
  Assets(const AudioAssets &audio_assets) = delete;
  ~Assets();

  /** Loads an *.ogg file into a buffer and caches it. */
  SharedAudioBuffer audio_buffer(const std::string &path);

  /** Remove unused buffers. */
  void clear_unused();
private:
  const std::string directory_;
  AudioBufferMap sounds_;
};
}
}