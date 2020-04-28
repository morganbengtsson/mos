#include <mos/gfx/target.hpp>

namespace mos::gfx {

std::atomic_uint Target::current_id_;

auto Target::id() const -> unsigned int { return id_; }

Target::Target() :
    id_(current_id_++) {
}
}
