#pragma once

namespace mos::gfx {
class Renderer;
}

namespace mos::gfx::gpu {

class Resource {
public:
  friend class mos::gfx::Renderer;
  static constexpr int invalid_id = -1;

  Resource(int id = invalid_id);

  auto valid() const -> bool;

  auto id() const -> int;
private:
  int id_{invalid_id};
};
}
