#ifndef MOS_AUDIO_ASSETS_H
#define MOS_AUDIO_ASSETS_H

#include <mos/audio/audio_buffer.hpp>
#include <mos/audio/audio_stream.hpp>
#include <map>
#include <memory>
#include <unordered_map>

namespace mos {
class AudioAssets final {
public:
  /**
 * @brief Container for audio buffers.
 */
  using AudioBufferMap =
  std::unordered_map<std::string, std::shared_ptr<AudioBuffer>>;

  /**
  * @brief Pair for SoundMap
  */
  using AudioBufferPair = std::pair<std::string, std::shared_ptr<AudioBuffer>>;

  using SharedAudioBuffer = std::shared_ptr<mos::AudioBuffer>;
  using SharedAudioStream = std::shared_ptr<mos::AudioStream>;

  AudioAssets(const std::string directory = "assets/");
  AudioAssets(const AudioAssets &audio_assets) = delete;
  ~AudioAssets();

  /**
  * Loads a *.ogg file into a Sound object, and caches it internally.
  *
  * @param path
  * @return Shared pointer to Sound object.
  */
  SharedAudioBuffer audio_buffer(const std::string &path);

  void clear_unused();
private:
  const std::string directory_;
  AudioBufferMap sounds_;
};
}
#endif //MOS_AUDIO_ASSETS_H
