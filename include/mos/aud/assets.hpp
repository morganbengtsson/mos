#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <utility>

#include <mos/aud/buffer.hpp>

namespace mos::aud {

/** Cache for heavy audio assets. */
class Assets final {
public:
  using Buffer_map = std::unordered_map<std::string, Shared_buffer>;
  using Buffer_pair = std::pair<std::string, Shared_buffer>;

  explicit Assets(const std::filesystem::path& directory = "assets/");
  ~Assets() = default;

  Assets(const Assets &assets) = delete;
  Assets(const Assets &&assets) = delete;

  auto operator=(const Assets &assets) -> Assets & = delete;
  auto operator=(const Assets &&assets) -> Assets & = delete;

  /** Loads an *.ogg file into a buffer and caches it. */
  auto audio_buffer(const std::filesystem::path &path) -> Shared_buffer;

  /** Remove unused buffers. */
  auto clear_unused() -> void;

  /** Remove all buffers */
  auto clear() -> void;

  /** Directory of assets */
  auto directory() const -> std::filesystem::path;

private:
  const std::string directory_;
  Buffer_map buffers_;
};
} // namespace mos::aud
