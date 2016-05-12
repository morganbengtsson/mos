#ifndef TYPES_HPP
#define TYPES_HPP

#include <memory>
#include <mos/audio/sound.hpp>
#include <mos/audio/stream.hpp>
#include <mos/graphics/mesh.hpp>

namespace mos {
using SharedSound = std::shared_ptr<Sound>;
using SharedMesh = std::shared_ptr<Mesh>;
using SharedStream = std::shared_ptr<Stream>;
}

#endif // TYPES_HPP
