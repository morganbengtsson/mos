#pragma once
#include <map>
#include <memory>
#include <unordered_map>
#include <mos/aud/buffer.hpp>
#include <mos/aud/stream.hpp>
#include <mos/aud/types.hpp>

namespace mos {
namespace aud {

class Assets final {
public:
  using BufferMap =
  std::unordered_map<std::string, SharedBuffer>;
  using BufferPair = std::pair<std::string, SharedBuffer>;

  Assets(const std::string directory = "assets/");
  Assets(const Assets &audio_assets) = delete;
  ~Assets();

  /** Loads an *.ogg file into a buffer and caches it. */
  SharedBuffer audio_buffer(const std::string &path);

  /** Remove unused buffers. */
  void clear_unused();
private:
  const std::string directory_;
  BufferMap sounds_;
};
}
}