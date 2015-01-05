#ifndef STREAM_HPP
#define STREAM_HPP

#include <iostream>

namespace mo{

class Stream
{
public:
    Stream(const std::string file_name);
    std::string file_name;
};

}
#endif // STREAM_HPP
