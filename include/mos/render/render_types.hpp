#ifndef MOS_RENDER_TYPES_HPP
#define MOS_RENDER_TYPES_HPP

#include <memory>
#include <mos/render/mesh.hpp>
#include <mos/render/render_target.hpp>

namespace mos {
using SharedMesh = std::shared_ptr<Mesh>;
using OptTarget = std::experimental::optional<RenderTarget>;
}

#endif // MOS_RENDER_TYPES_HPP
