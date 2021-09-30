#pragma once

#include <chrono>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx-presets.h>

#include <mos/aud/buffer.hpp>
#include <mos/al/resource.hpp>

namespace mos::al {

/* Audio buffer living on the APU */
class Buffer final : public Resource {
  friend class Renderer;

private:
  Buffer(const mos::aud::Buffer &buffer);
};
}
