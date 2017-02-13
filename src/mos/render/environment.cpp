#include <mos/render/environment.hpp>

namespace mos {

Environment::Environment(const SharedTextureCube &texture, const RenderBox &box) : texture(texture), box(box) {
}

}