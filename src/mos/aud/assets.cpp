#include <mos/aud/assets.hpp>

namespace mos {
namespace aud {

Assets::Assets(const std::string &directory) : directory_(directory) {}

Shared_buffer Assets::audio_buffer(const std::string &path) {
  if (buffers_.find(path) == buffers_.end()) {
    buffers_.insert(Buffer_pair(path, Buffer::load(directory_ + path)));
    return buffers_.at(path);
  } else {
    return buffers_.at(path);
  }
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

std::string Assets::directory() const{
  return directory_;
}
}
}
