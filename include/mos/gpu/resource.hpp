#pragma once

namespace mos::gfx {
class Renderer;
} // namespace mos::gfx

namespace mos::gpu {

class Resource {
public:
  friend class mos::gfx::Renderer;
  static constexpr int invalid_id = -1;

  Resource(int id = invalid_id);

  [[nodiscard]] auto valid() const -> bool;

  [[nodiscard]] auto id() const -> int;
private:
  int id_{invalid_id};
};
} // namespace mos::gpu
