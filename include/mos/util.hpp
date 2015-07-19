/* 
 * File:   util.h
 * Author: morgan
 *
 * Created on April 13, 2014, 8:43 PM
 */

#ifndef MOS_UTIL_H
#define	MOS_UTIL_H

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
    explicit ClockwiseSorter(const T center) : center(center) {
    }

    bool operator()(const T a, const T b) const {

        float det = (a.x - center.x) * (b.y - center.y) - (b.x - center.x) * (a.y - center.y);
        if (det < 0){
            return true;
        }
        if (det > 0){
            return false;
        }
    }
};

std::string inline text(const std::string path){
    std::ifstream file(path);
    if (!file.is_open()){
        throw std::runtime_error("Could not open " + path + ".");
    }
    std::string source((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());

    return source;
}

template <typename T>
std::vector<T>& operator+=(std::vector<T>& a, const std::vector<T>& b)
{
    a.insert(a.end(), b.begin(), b.end());
    return a;
}

template <typename T>
std::vector<T>& operator+=(std::vector<T>& aVector, const T& aObject)
{
    aVector.push_back(aObject);
    return aVector;
}

template <typename T>
std::vector<T> operator+(const std::vector<T>& a, const std::vector<T>& b)
{
    assert(a.size() == b.size());

    std::vector<T> result;
    result.reserve(a.size());

    std::transform(a.begin(), a.end(), b.begin(),
                   std::back_inserter(result), std::plus<T>());
    return result;
}

template <typename T>
std::vector<T> operator-(const std::vector<T>& a, const std::vector<T>& b)
{
    assert(a.size() == b.size());

    std::vector<T> result;
    result.reserve(a.size());

    std::transform(a.begin(), a.end(), b.begin(),
                   std::back_inserter(result), std::minus<T>());
    return result;
}

template <typename T>
std::vector<T> operator*(const std::vector<T>& a, const std::vector<T>& b)
{
    assert(a.size() == b.size());

    std::vector<T> result;
    result.reserve(a.size());

    std::transform(a.begin(), a.end(), b.begin(),
                   std::back_inserter(result), std::multiplies<T>());
    return result;
}

template <typename T>
std::vector<T> operator/(const std::vector<T>& a, const std::vector<T>& b)
{
    assert(a.size() == b.size());

    std::vector<T> result;
    result.reserve(a.size());

    std::transform(a.begin(), a.end(), b.begin(),
                   std::back_inserter(result), std::divides<T>());
    return result;
}
}

#endif	/* MOS_UTIL_H */

