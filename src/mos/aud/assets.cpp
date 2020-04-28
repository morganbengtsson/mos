#include <mos/aud/assets.hpp>

namespace mos {
namespace aud {

Assets::Assets(std::string directory) : directory_(std::move(directory)) {}

auto Assets::audio_buffer(const std::string &path) -> Shared_buffer {
  if (buffers_.find(path) == buffers_.end()) {
    buffers_.insert(Buffer_pair(path, Buffer::load(directory_ + path)));
  }
  return buffers_.at(path);
}

void Assets::clear_unused() {
  for (auto it = buffers_.begin(); it != buffers_.end();) {
    if (it->second.use_count() <= 1) {
      buffers_.erase(it++);
    } else {
      ++it;
    }
  }
}
void Assets::clear() {
  buffers_.clear();
}

auto Assets::directory() const -> std::string {
  return directory_;
}
}
}
