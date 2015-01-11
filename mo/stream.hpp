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
    Stream(const std::string file_name);
    ~Stream();

    int buffer_size;
    StreamData read();
    int sample_rate() const;
    int channels() const;
    void seek_start();
private:
    int samples_left;
    int size;
    std::string file_name;
    stb_vorbis * vorbis_stream;
    stb_vorbis_info vorbis_info;
};




}
#endif // STREAM_HPP
