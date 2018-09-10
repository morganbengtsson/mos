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
  explicit Buffer_source(const Shared_buffer &buffer = nullptr,
               const Source &source = Source());
  ~Buffer_source() = default;

  /** Play the source. */
  void input(const float dt);

  Shared_buffer buffer;
  Source source;
private:
  float time_;
};
}
}
