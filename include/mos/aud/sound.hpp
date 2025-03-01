#pragma once

#include <glm/glm.hpp>
#include <json.hpp>

#include <mos/aud/assets.hpp>
#include <mos/aud/buffer.hpp>
#include <mos/aud/source.hpp>

namespace mos::aud {

/** Audio buffer and audio source combined. */
class Sound final {
public:
  static auto load(const nlohmann::json &json, Assets &assets,
                   const glm::mat4 &parent_transform = glm::mat4(1.0F))
      -> Sound;

  explicit Sound(Shared_buffer buffer = nullptr, Source source = Source());
  Shared_buffer buffer;
  Source source;
};
} // namespace mos::aud
