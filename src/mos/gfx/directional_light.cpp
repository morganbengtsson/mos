#include <mos/gfx/directional_light.hpp>
#include <filesystem>
#include <json.hpp>
#include <mos/util.hpp>

mos::gfx::Directional_light::Directional_light(const std::string &directory,
                                               const std::string &path,
                                               const glm::mat4 &parent_transform)
{
  using json = nlohmann::json;
  if (!path.empty()) {
    std::filesystem::path fpath = path;
    auto value = json::parse(mos::text(directory + fpath.generic_string()));

    auto transform = parent_transform * jsonarray_to_mat4(value["transform"]);
    position = glm::vec3(transform[3]);
    direction = glm::normalize(glm::vec3(transform * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)));

    std::string t = value["light"];
    auto data_value = json::parse(mos::text(directory + t));

    color = glm::vec3(data_value["color"][0],
                      data_value["color"][1],
                      data_value["color"][2]);
    strength = data_value["strength"];
  }
}
