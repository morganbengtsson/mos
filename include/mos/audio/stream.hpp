#ifndef MOS_STREAM_HPP
#define MOS_STREAM_HPP

#include <array>

#include "stb_vorbis.h"

namespace mos {

/**
 * @brief The Stream class, for audio streaming from file.
 */
class Stream {
public:
  /**
   * @brief Stream constructor.
   * @param file_name
   */
  explicit Stream(const std::string file_name);

  /**
   * @brief Copy constructor.
   * @param stream
   */
  Stream(const mos::Stream &stream);
  ~Stream();

  /**
   * @brief buffer_size
   */
  static const int buffer_size = 4096 * 8;

  /**
   * @brief read chunk of data from the stream.
   * @return Chunk of shorts.
   */
  std::array<short, buffer_size> read();

  /**
   * @brief done
   * @return True if the stream has no more data.
   */
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
   * @brief seek_start Starts the stream from the beginning.
   */
  void seek_start();

  /**
   * @brief unique id
   * @return id
   */
  unsigned int id() const;

private:
  unsigned int id_;
  int samples_left_;
  int size_;
  std::string file_name_;
  stb_vorbis *vorbis_stream_;
  stb_vorbis_info vorbis_info_;
};
}
#endif // MOS_STREAM_HPP
