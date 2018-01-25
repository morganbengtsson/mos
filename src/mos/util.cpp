#include <mos/util.hpp>
#include <fstream>
#include <stdexcept>

std::string mos::text(const std::string path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open " + path + ".");
  }
  std::string source((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());

  return source;
}


std::vector<std::string> &mos::split(const std::string & s, char delim, std::vector<std::string> & elems) {
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

std::vector<std::string> mos::split(const std::string & s, char delim) {
  std::vector<std::string> elems;
  split(s, delim, elems);
  return elems;
}
glm::vec3 mos::position(const glm::mat4 &mat) {
  return glm::vec3(mat[3][0], mat[3][1], mat[3][2]);
}

glm::mat4 mos::jsonarray_to_mat4(const nlohmann::json &array) {
  glm::mat4 transform(1.0f);
  if (!array.is_null()) {
    std::vector<float> nums(array.begin(), array.end());
    transform = glm::make_mat4x4(nums.data());
  }
  return transform;
}
int mos::now_ms() {
  using namespace std::chrono;
  auto now = system_clock::now();
  return duration_cast<milliseconds>(now.time_since_epoch()).count();
}