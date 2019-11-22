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
class Sound final {
public:
  Sound(const nlohmann::json &json, Assets &assets,
        const glm::mat4 &parent_transform = glm::mat4(1.0f));
  explicit Sound(Shared_buffer buffer = nullptr,
                         Source source = Source());
  Shared_buffer buffer;
  Source source;
};
}
}
