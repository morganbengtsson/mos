#include <mos/aud/sound.hpp>
#include <iostream>
#include <mos/util.hpp>

namespace mos {
namespace aud {

Sound::Sound(const nlohmann::json &json, Assets &assets,
                             const glm::mat4 &parent_transform) {
  auto parsed = json;
  if (parsed.is_string()) {
    std::cout << "Loading: " << parsed << std::endl;
    std::string path = parsed;
    parsed = nlohmann::json::parse(mos::text(assets.directory() + path));
  }
  std::string sound_data_path;
  if (!parsed["sound"].is_null()) {
    sound_data_path = parsed.value("sound", "");
  }
  auto sound_data = nlohmann::json::parse(mos::text(assets.directory() + sound_data_path));

  std::string buffer_path = sound_data["sound"];
  buffer = assets.audio_buffer(buffer_path);

  auto transform = parent_transform * jsonarray_to_mat4(parsed["transform"]);

  float gain = sound_data["volume"];
  float pitch = sound_data["pitch"];

  source = Source(glm::vec3(transform[3]), glm::vec3(0.0f), pitch, gain);
}

Sound::Sound(Shared_buffer buffer,
                             Source source)
    : buffer(std::move(buffer)), source(source) {}

void Sound::input(const float dt) {
  if (source.playing) {
    time_ += dt;
  }
  if (time_ > (buffer->duration() / source.pitch) && !source.loop) {
    source.playing = false;
    time_ = .0f;
  }
}
}
}
