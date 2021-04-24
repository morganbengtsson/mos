#include <map>
#include <mos/gfx/texture.hpp>
#include <stdexcept>

namespace mos::gfx {
std::atomic_int Texture::current_id_;

Texture::Texture(const Filter &filter, const Wrap &wrap, const bool generate_mipmaps)
    : generate_mipmaps(generate_mipmaps), filter(filter), wrap(wrap),
      modified(std::chrono::system_clock::now()), id_(current_id_++) {}

auto Texture::id() const -> int { return id_; }

} // namespace mos::gfx
