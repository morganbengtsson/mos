#ifndef MOS_UTIL_H
#define MOS_UTIL_H

#include <fstream>
#include <functional>
#include <algorithm>

namespace mos {
template <class T>
/**
 * @brief The ClockwiseSorter class
 */
class ClockwiseSorter {
private:
  T center;

public:
  /**
   * @brief ClockwiseSorter
   * @param center
   */
  explicit ClockwiseSorter(const T center) : center(center) {}

  /**
   * @brief operator ()
   * @param a
   * @param b
   * @return
   */
  bool operator()(const T a, const T b) const {

    float det = (a.x - center.x) * (b.y - center.y) -
                (b.x - center.x) * (a.y - center.y);
    if (det < 0) {
      return true;
    }
    if (det > 0) {
      return false;
    }
  }
};

/**
 * @brief Load text from file.
 * @param path Full path to the file.
 * @return String with all content.
 */
std::string inline text(const std::string path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open " + path + ".");
  }
  std::string source((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());

  return source;
}

template <typename T>
/**
 * @brief operator +=
 * @param a
 * @param b
 * @return
 */
std::vector<T> &operator+=(std::vector<T> &a, const std::vector<T> &b) {
  a.insert(a.end(), b.begin(), b.end());
  return a;
}

template <typename T>
/**
 * @brief operator +=
 * @param aVector
 * @param aObject
 * @return
 */
std::vector<T> &operator+=(std::vector<T> &aVector, const T &aObject) {
  aVector.push_back(aObject);
  return aVector;
}

template <typename T>
/**
 * @brief operator +
 * @param a
 * @param b
 * @return
 */
std::vector<T> operator+(const std::vector<T> &a, const std::vector<T> &b) {
  assert(a.size() == b.size());

  std::vector<T> result;
  result.reserve(a.size());
  /*
std::transform(a.begin(), a.end(), b.begin(),
             std::back_inserter(result), std::plus<T>());*/
  return result;
}

template <typename T>
/**
 * @brief operator -
 * @param a
 * @param b
 * @return
 */
std::vector<T> operator-(const std::vector<T> &a, const std::vector<T> &b) {
  assert(a.size() == b.size());

  std::vector<T> result;
  result.reserve(a.size());
  /*
std::transform(a.begin(), a.end(), b.begin(),
             std::back_inserter(result), std::minus<T>());*/
  return result;
}

template <typename T>
/**
 * @brief operator *
 * @param a
 * @param b
 * @return
 */
std::vector<T> operator*(const std::vector<T> &a, const std::vector<T> &b) {
  assert(a.size() == b.size());

  std::vector<T> result;
  result.reserve(a.size());
  /*
std::transform(a.begin(), a.end(), b.begin(),
             std::back_inserter(result), std::multiplies<T>());*/
  return result;
}

template <typename T>
/**
 * @brief operator /
 * @param a
 * @param b
 * @return
 */
std::vector<T> operator/(const std::vector<T> &a, const std::vector<T> &b) {
  assert(a.size() == b.size());

  std::vector<T> result;
  result.reserve(a.size());
  /*
std::transform(a.begin(), a.end(), b.begin(),
             std::back_inserter(result), std::divides<T>());*/
  return result;
}
}

#endif /* MOS_UTIL_H */
