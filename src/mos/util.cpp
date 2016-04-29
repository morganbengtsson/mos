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
