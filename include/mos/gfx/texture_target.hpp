#pragma once

#include <mos/gfx/texture_2d.hpp>
#include <mos/gfx/target.hpp>
#include "camera.hpp"

namespace mos {
namespace gfx {

/** Off screen rendertarget. */
struct Texture_target {
  Texture_target(){
    texture = std::make_shared<Texture_2D>(Texture_2D(128, 128));
  }
  mos::gfx::Camera camera;
  mos::gfx::Shared_texture_2D texture;
  mos::gfx::Target target;
};

}
}
