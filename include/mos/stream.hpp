#ifndef MOS_STREAM_HPP
#define MOS_STREAM_HPP

#include <array>
#include "stb_vorbis.h"

namespace mos {

struct StreamData {
    short samples[4096*8];
    bool done;
};

/**
 * @brief The Stream class, for audio streaming from file.
 */
class Stream
{
public:
    /**
     * @brief Stream constructor.
     * @param file_name
     */
    explicit Stream(const std::string file_name_);
    Stream(const mos::Stream & stream);
    ~Stream();

    int buffer_size;

    /**
     * @brief read chunk of data from the stream.
     * @return Chunk of shorts.
     */
    std::array<short, 4096*8> read();

    bool done() const;

    /**
     * @brief sample_rate
     * @return
     */
    int sample_rate() const;

    /**
     * @brief Number of channels, usually one or two.
     * @return channels (1 or 2)
     */
    int channels() const;

    /**
     * @brief seek_start starts the stream from the beginning.
     */
    void seek_start();
private:
    int samples_left_;
    int size_;
    std::string file_name_;
    stb_vorbis * vorbis_stream_;
    stb_vorbis_info vorbis_info_;
};




}
#endif // MOS_STREAM_HPP
