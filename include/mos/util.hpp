#ifndef MOS_UTIL_H
#define MOS_UTIL_H

#include <string>
#include <rapidjson/document.h>

namespace mos {
/**
* @brief Load a json document.
* @param path Full path.
* @return Rapidjson document.
*/
rapidjson::Document document(const std::string &path);

/**
 * @brief Load text from file.
 * @param path Full path.
 * @return String with all content.
 */
std::string text(const std::string path);
}

#endif /* MOS_UTIL_H */
