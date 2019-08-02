#pragma once
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <mos/aud/buffer.hpp>
#include <mos/aud/source.hpp>
#include <mos/aud/assets.hpp>
#include <json.hpp>

namespace mos {
namespace aud {

/** Audio buffer and audio source combined. */
class Speaker final {
public:
  Speaker(const nlohmann::json &json, Assets &assets,
                const glm::mat4 &parent_transform = glm::mat4(1.0f));
  explicit Speaker(Shared_buffer buffer = nullptr,
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
