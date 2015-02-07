/* 
 * File:   util.h
 * Author: morgan
 *
 * Created on April 13, 2014, 8:43 PM
 */

#ifndef MO_UTIL_H
#define	MO_UTIL_H

#include <string>
#include <fstream>
#include <exception>

namespace mo {
    template <class T>
    class ClockwiseSorter {
    private:
        T center;
    public:
        ClockwiseSorter(const T center) : center(center) {
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
}

#endif	/* MO_UTIL_H */

