#pragma once

#include <map>
#include <memory>
#include <unordered_map>
#include <mos/aud/buffer.hpp>
#include <mos/aud/stream.hpp>

namespace mos::aud {

/** Cache for heavy audio assets. */
class Assets final {
public:
  using Buffer_map =
  std::unordered_map<std::string, Shared_buffer>;
  using Buffer_pair = std::pair<std::string, Shared_buffer>;

  explicit Assets(std::string directory = "assets/");
  ~Assets() = default;

  Assets(const Assets &assets) = delete;
  Assets(const Assets &&assets) = delete;

  Assets & operator=(const Assets & assets) = delete;
  Assets & operator=(const Assets && assets) = delete;

  /** Loads an *.ogg file into a buffer and caches it. */
  auto audio_buffer(const std::string &path) -> Shared_buffer;

  /** Remove unused buffers. */
  auto clear_unused() -> void;

  /** Remove all buffers */
  auto clear() -> void;

  /** Directory of assets */
  auto directory() const -> std::string;

private:
  const std::string directory_;
  Buffer_map buffers_;
};
}

