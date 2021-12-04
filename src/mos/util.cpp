#include <mos/util.hpp>
#include <fstream>
#include <stdexcept>
#include <glm/glm.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <mos/sim/ray.hpp>

auto mos::text(const std::string &path) -> std::string {
  std::ifstream file(path);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open " + path + ".");
  }
  std::string source((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());

  return source;
}


auto mos::split(const std::string & s, char delim, std::vector<std::string> & elems) -> std::vector<std::string> & {
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

auto mos::split(const std::string & s, char delim) -> std::vector<std::string> {
  std::vector<std::string> elems;
  split(s, delim, elems);
  return elems;
}
auto mos::position(const glm::mat4 &mat) -> glm::vec3 {
  return glm::vec3(mat[3][0], mat[3][1], mat[3][2]);
}

auto mos::jsonarray_to_mat4(const nlohmann::json &array) -> glm::mat4 {
  glm::mat4 transform(1.0F);
  if (!array.is_null()) {
    std::vector<float> nums(array.begin(), array.end());
    transform = glm::make_mat4x4(nums.data());
  }
  return transform;
}
auto mos::now_ms() -> int {
  using namespace std::chrono;
  auto now = system_clock::now();
  return int(duration_cast<milliseconds>(now.time_since_epoch()).count());
}
auto mos::un_project(const glm::vec2 &position,
                              const glm::mat4 &view,
                              const glm::mat4 &projection,
                              const glm::uvec2 &resolution) -> sim::Ray {
    const auto position0 =
        glm::unProject(glm::vec3(position.x,
                                 resolution.y - position.y, 0.0F),
                       view, projection,
                       glm::vec4(0.0F, 0.0F, resolution.x, resolution.y));

    const auto position1 =
        glm::unProject(glm::vec3(position.x,
                                 resolution.y - position.y, 1.0F),
                       view, projection,
                       glm::vec4(0.0F, 0.0F, resolution.x, resolution.y));

    return mos::sim::Ray(position0, glm::normalize(position1 - position0));

}
auto mos::jsonarray_to_quat(const nlohmann::json &array) -> glm::quat {
  return glm::quat(array[0], array[1], array[2], array[3]);
}

auto mos::jsonarray_to_vec4(const nlohmann::json &array) -> glm::vec4 {
  return glm::vec4(array[0], array[1], array[2], array[3]);
}

auto mos::jsonarray_to_vec3(const nlohmann::json &array) -> glm::vec3 {
  return glm::vec3(array[0], array[1], array[2]);
}

auto mos::hex_color(const int hex) -> glm::vec3 {
  glm::vec3 color;
  constexpr auto max = std::numeric_limits<unsigned char>::max();
  color.r = ((hex >> 16) & 0xFF) / max;
  color.g = ((hex >> 8) & 0xFF) / max;
  color.b = ((hex) & 0xFF) / max;
  return color;
}
