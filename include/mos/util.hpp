#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <json.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <mos/sim/ray.hpp>
#include <chrono>

namespace mos {

/**
 * @brief Load text from file.
 * @param path Full path.
 * @return String with all content.
 */
std::string text(const std::string &path);

std::vector<std::string> &split(const std::string &s, char delim,
                                std::vector<std::string> &elems);

std::vector<std::string> split(const std::string &s, char delim);

glm::mat4 jsonarray_to_mat4(const nlohmann::json &array);
glm::vec3 jsonarray_to_vec3(const nlohmann::json &array);
glm::quat jsonarray_to_quat(const nlohmann::json &array);
glm::vec4 jsonarray_to_vec4(const nlohmann::json &array);


glm::vec3 position(const glm::mat4 &mat);

int now_ms();

mos::sim::Ray un_project(const glm::vec2 &position,
                         const glm::mat4 &view,
                         const glm::mat4 &projection,
                         const glm::uvec2 &resolution);

auto hex_color(const int hex) -> glm::vec4;

}
