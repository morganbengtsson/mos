#pragma once
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <mos/aud/buffer.hpp>
#include <mos/aud/source.hpp>

namespace mos {
namespace aud {

/** Audio buffer and audio source combined. */
class Buffer_source final {
public:
  explicit Buffer_source(Shared_buffer buffer = nullptr,
                         Source source = Source());

  /** Play the source. */
  void input(float dt);

  Shared_buffer buffer;
  Source source;
private:
  float time_{0.0f};
};
}
}
