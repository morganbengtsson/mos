#ifndef MOS_UTIL_H
#define MOS_UTIL_H

#include <string>
#include <sstream>
#include <vector>
#include <json.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace mos {

/**
 * @brief Load text from file.
 * @param path Full path.
 * @return String with all content.
 */
std::string text(const std::string path);

std::vector<std::string> &split(const std::string &s, char delim,
                                std::vector<std::string> &elems);

std::vector<std::string> split(const std::string &s, char delim);


glm::mat4 jsonarray_to_mat4(const nlohmann::json &array);

glm::vec3 position(const glm::mat4 &mat);


}
#endif /* MOS_UTIL_H */
