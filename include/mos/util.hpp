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

/** UTF-8 code point to string */
std::string utf8_str(unsigned int codepoint) {
  char c[5]={ 0x00,0x00,0x00,0x00,0x00 };
  if     (codepoint<=0x7F) { c[0] = codepoint;  }
  else if(codepoint<=0x7FF) { c[0] = (codepoint>>6)+192; c[1] = (codepoint&63)+128; }
  else if(0xd800<=codepoint && codepoint<=0xdfff) {} //invalid block of utf8
  else if(codepoint<=0xFFFF) { c[0] = (codepoint>>12)+224; c[1]= ((codepoint>>6)&63)+128; c[2]=(codepoint&63)+128; }
  else if(codepoint<=0x10FFFF) { c[0] = (codepoint>>18)+240; c[1] = ((codepoint>>12)&63)+128; c[2] = ((codepoint>>6)&63)+128; c[3]=(codepoint&63)+128; }
  return std::string(c);
}

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

glm::vec3 position(const glm::mat4 &mat);

int now_ms();

mos::sim::Ray un_project(const glm::vec2 &position,
                         const glm::mat4 &view,
                         const glm::mat4 &projection,
                         const glm::uvec2 &resolution);
}
