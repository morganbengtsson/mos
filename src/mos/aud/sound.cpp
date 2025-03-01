#include <mos/aud/sound.hpp>
#include <iostream>
#include <mos/util.hpp>
#include <spdlog/spdlog.h>

namespace mos::aud {

auto Sound::load(const nlohmann::json &json, Assets &assets,
                 const glm::mat4 &parent_transform) -> Sound {
  auto parsed = json;
  if (parsed.is_string()) {
    spdlog::info("Loading: {}", parsed);
    std::string const path = parsed;
    auto full_path = assets.directory();
    full_path += path;
    parsed = nlohmann::json::parse(mos::text(full_path));
  }
  std::string sound_data_path;
  if (!parsed["sound"].is_null()) {
    sound_data_path = parsed.value("sound", "");
  }
  auto full_path = assets.directory();
  full_path += sound_data_path;
  auto sound_data = nlohmann::json::parse(mos::text(full_path));

  std::string buffer_path = sound_data["sound"];
  const auto buffer = assets.audio_buffer(buffer_path);

  auto transform = parent_transform * jsonarray_to_mat4(parsed["transform"]);

  float gain = sound_data["volume"];
  float pitch = sound_data["pitch"];

  const auto source = Source(glm::vec3(transform[3]), glm::vec3(0.0F), pitch, gain);
  return Sound(buffer, source);
}

Sound::Sound(Shared_buffer buffer,
                             Source source)
    : buffer(std::move(buffer)), source(source) {}
} // namespace mos::aud
