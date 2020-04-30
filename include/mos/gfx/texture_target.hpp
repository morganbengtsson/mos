#pragma once

#include <mos/gfx/texture_2d.hpp>
#include <mos/gfx/target.hpp>
#include <mos/gfx/camera.hpp>

namespace mos::gfx {

/** Off screen rendertarget. */
struct Texture_target {
  Texture_target(){
    texture = std::make_shared<Texture_2D>(Texture_2D(128, 128));
  }
  Camera camera;
  Shared_texture_2D texture;
  Target target;
};
}
