#include <mos/aud/assets.hpp>

namespace mos::aud {

Assets::Assets(std::filesystem::path directory) : directory_(std::move(directory)) {}

auto Assets::audio_buffer(const std::filesystem::path &path) -> Shared_buffer {
  if (buffers_.find(path) == buffers_.end()) {
    auto full_path = directory_;
    full_path += path;
    buffers_.insert(Buffer_pair(path, std::make_shared<Buffer>(Buffer::load(full_path))));
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

auto Assets::directory() const -> std::filesystem::path {
  return directory_;
}
}
