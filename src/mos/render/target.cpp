#include <mos/render/target.hpp>
#include <mos/render/texture_cube.hpp>

namespace mos {
namespace gfx {

std::atomic_uint Target::current_id_;

unsigned int Target::id() const { return id_; }

Target::Target() :
    id_(current_id_++) {
}
}
}