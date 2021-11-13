#include <mos/gfx/directional_light.hpp>
#include <filesystem>
#include <json.hpp>
#include <mos/util.hpp>

auto mos::gfx::Directional_light::load(const std::string &directory,
                                  const std::string &path,
                                       const glm::mat4 &parent_transform) -> mos::gfx::Directional_light {
  using json = nlohmann::json;
  if (!path.empty()) {
    std::filesystem::path fpath = path;
    auto value = json::parse(mos::text(directory + fpath.generic_string()));

    auto transform = parent_transform * jsonarray_to_mat4(value["transform"]);
    auto position = glm::vec3(transform[3]);
    auto direction = glm::normalize(
        glm::vec3(transform * glm::vec4(0.0F, 0.0F, -1.0F, 0.0F)));

    std::string t = value["light"];
    auto data_value = json::parse(mos::text(directory + t));

    auto color = glm::vec3(data_value["color"][0], data_value["color"][1],
                      data_value["color"][2]);
    auto strength = data_value["strength"];
    return Directional_light{position, direction, color, strength};
  }
  throw std::runtime_error("Invalid path for Directional light");
}
