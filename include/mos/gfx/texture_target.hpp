#pragma once

#include <mos/gfx/texture_2d.hpp>
#include <mos/gfx/target.hpp>
#include "camera.hpp"

namespace mos {
namespace gfx {

struct TextureTarget {
  TextureTarget(){
    texture = std::make_shared<Texture2D>(Texture2D(128, 128));
  }
  mos::gfx::Camera camera;
  mos::gfx::SharedTexture2D texture;
  mos::gfx::Target target;
};

}
}
