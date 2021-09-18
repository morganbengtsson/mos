#pragma once

#include <mos/gfx/texture_2d.hpp>
#include <mos/gpu/resource.hpp>

namespace mos::gfx::gpu {

class Texture_2D final : public Resource {
  friend class Model;
  friend class Material;
  friend class mos::gfx::Renderer;

public:
  Texture_2D();

private:
  explicit Texture_2D(const mos::gfx::Texture_2D &texture);
  explicit Texture_2D(const mos::gfx::Shared_texture_2D &shared_texture);
};
}
