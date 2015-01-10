#ifndef STREAM_HPP
#define STREAM_HPP

#include <iostream>
#include <memory>
#include <array>
#include <tuple>
#include <glm/glm.hpp>

#include "sound.hpp"
#include "stb_vorbis.h"

namespace mo {

struct StreamData {
    short samples[4096*32];
    bool done;
};

class Stream
{
public:
    Stream(const std::string file_name,
           const glm::vec3 position = glm::vec3(0.0f),
           const bool loop = false);
    //Stream(const Stream & original);
    ~Stream();
    std::string file_name;
    int samples_left;
    int size;
    int buffer_size;
    int channels;
    stb_vorbis * vorbis_stream;
    stb_vorbis_info vorbis_info;
    bool loop;
    glm::vec3 position;
    StreamData read();
    bool playing;
};



}
#endif // STREAM_HPP
