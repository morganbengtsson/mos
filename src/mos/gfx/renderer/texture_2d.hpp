#pragma once

#include <mos/gfx/texture_2d.hpp>

class Texture_2D final {
  friend class Renderer;

private:
  explicit Texture_2D(const mos::gfx::Texture_2D &texture);

  const int id;
};
