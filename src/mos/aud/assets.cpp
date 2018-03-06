#include <mos/aud/assets.hpp>

namespace mos {
namespace aud {

Assets::Assets(const std::string directory) : directory_(directory) {}
Assets::~Assets() {}
std::shared_ptr <Buffer>
Assets::audio_buffer(const std::string &path) {
  if (sounds_.find(path) == sounds_.end()) {
    sounds_.insert(BufferPair(path, Buffer::load(directory_ + path)));
    return sounds_.at(path);
  } else {
    return sounds_.at(path);
  }
}
void Assets::clear_unused() {
  for (auto it = sounds_.begin(); it != sounds_.end();) {
    if (it->second.use_count() <= 1) {
      sounds_.erase(it++);
    } else {
      ++it;
    }
  }
}
}
}
