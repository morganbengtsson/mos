#pragma once
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <mos/aud/types.hpp>
#include <mos/aud/buffer.hpp>
#include <mos/aud/source.hpp>

namespace mos {
namespace aud {

/** Audio buffer and audio source combined. */
class BufferSource final {
public:
  BufferSource(const SharedBuffer &buffer = nullptr,
               const Source &source = Source());
  ~BufferSource();

  void update(const float dt);

  SharedBuffer buffer;
  Source source;
private:
  float time_;
};
}
}
