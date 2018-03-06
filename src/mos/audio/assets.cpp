#include <mos/audio/audio_assets.h>

namespace mos {

AudioAssets::AudioAssets(const std::string directory) : directory_(directory) {}
AudioAssets::~AudioAssets() {}
std::shared_ptr<AudioBuffer>
AudioAssets::audio_buffer(const std::string &path) {
  if (sounds_.find(path) == sounds_.end()) {
    sounds_.insert(AudioBufferPair(path, AudioBuffer::load(directory_ + path)));
    return sounds_.at(path);
  } else {
    return sounds_.at(path);
  }
}
void AudioAssets::clear_unused() {
  for (auto it = sounds_.begin(); it != sounds_.end();) {
    if (it->second.use_count() <= 1) {
      sounds_.erase(it++);
    } else {
      ++it;
    }
  }
}

}
