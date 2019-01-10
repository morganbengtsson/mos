#pragma once
#include <map>
#include <memory>
#include <unordered_map>
#include <mos/aud/buffer.hpp>
#include <mos/aud/stream.hpp>

namespace mos {
namespace aud {

/** Cache for heavy audio assets. */
class Assets final {
public:
  using Buffer_map =
  std::unordered_map<std::string, Shared_buffer>;
  using Buffer_pair = std::pair<std::string, Shared_buffer>;

  explicit Assets(const std::string &directory = "assets/");
  Assets(const Assets &assets) = delete;
  ~Assets() = default;

  /** Loads an *.ogg file into a buffer and caches it. */
  Shared_buffer audio_buffer(const std::string &path);

  /** Remove unused buffers. */
  void clear_unused();

  /** Remove all buffers */
  void clear();

  /** Directory of assets */
  std::string directory() const;

private:
  const std::string directory_;
  Buffer_map buffers_;
};
}
}
