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

// Load full string from text file
auto text(const std::string &path) -> std::string;

[[deprecated]]
auto split(const std::string &s, char delim,
      std::vector<std::string> &elems) -> std::vector<std::string> &;
[[deprecated]]
auto split(const std::string &s, char delim) -> std::vector<std::string>;

auto jsonarray_to_mat4(const nlohmann::json &array) -> glm::mat4;
auto jsonarray_to_vec3(const nlohmann::json &array) -> glm::vec3;
auto jsonarray_to_quat(const nlohmann::json &array) -> glm::quat;
auto jsonarray_to_vec4(const nlohmann::json &array) -> glm::vec4;

[[deprecated]]
auto position(const glm::mat4 &mat) -> glm::vec3;

int now_ms();

auto un_project(const glm::vec2 &position,
                         const glm::mat4 &view,
                         const glm::mat4 &projection,
                const glm::uvec2 &resolution) -> mos::sim::Ray;

auto hex_color(const int hex) -> glm::vec3;

}
