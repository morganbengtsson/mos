#ifndef STREAM_HPP
#define STREAM_HPP

#include <iostream>

#include "sound.hpp"

namespace mo{

class Stream
{
public:
    Stream(const std::string file_name);
    std::string file_name;
    Sound buffer1;
    Sound buffer2;
    int samples_left;
    int size;
};

}
#endif // STREAM_HPP
