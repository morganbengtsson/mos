#include <mos/aud/buffer_source.hpp>
#include <iostream>
#include <mos/util.hpp>

namespace mos {
namespace aud {

Buffer_source::Buffer_source(const nlohmann::json &json, Assets &assets,
                             const glm::mat4 &parent_transform) {
  auto parsed = json;
  if (parsed.is_string()) {
    std::cout << "Loading: " << parsed << std::endl;
    std::string path = parsed;
    parsed = nlohmann::json::parse(mos::text(assets.directory() + path));
  }
  std::string speaker_data_path;
  if (!parsed["speaker_data"].is_null()) {
    speaker_data_path = parsed.value("speaker_data", "");
  }
  auto speaker_data = nlohmann::json::parse(mos::text(assets.directory() + speaker_data_path));

  std::string buffer_path = speaker_data["sound"];
  buffer = assets.audio_buffer(buffer_path);

  auto transform = parent_transform * jsonarray_to_mat4(parsed["transform"]);

  float gain = speaker_data["volume"];
  float pitch = speaker_data["pitch"];

  source = Source(glm::vec3(transform[3]), glm::vec3(0.0f), pitch, gain);
}

Buffer_source::Buffer_source(Shared_buffer buffer,
                             Source source)
    : buffer(std::move(buffer)), source(source) {}

void Buffer_source::input(const float dt) {
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
