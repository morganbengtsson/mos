#pragma once

#include <mos/gfx/texture_2d.hpp>
#include <mos/gfx/target.hpp>

namespace mos {
namespace gfx {

struct Offscreen {
  Offscreen(){
    texture = std::make_shared<Texture2D>(Texture2D(128, 128));
  }
  mos::gfx::SharedTexture2D texture;
  mos::gfx::Target target;
};

}
}
