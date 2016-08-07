#ifndef MOS_AUDIO_STREAM_HPP
#define MOS_AUDIO_STREAM_HPP

#include <array>
#include <string>
#include <atomic>

#include <stb_vorbis.h>

namespace mos {

/**
 * @brief The Stream class, for audio streaming from file.
 */
class AudioStream final{
public:
  /**
   * @brief Stream constructor.
   * @param file_name
   */
  explicit AudioStream(const std::string &path);

  ~AudioStream();

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
  static std::atomic_uint current_id_;
  unsigned int id_;
  int samples_left_;
  int size_;
  std::string file_name_;
  stb_vorbis *vorbis_stream_;
  stb_vorbis_info vorbis_info_;
};
}
#endif // MOS_AUDIO_STREAM_HPP
