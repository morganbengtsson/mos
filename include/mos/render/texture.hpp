#ifndef MOS_TEXTURE2D_H
#define MOS_TEXTURE2D_H

#include <atomic>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

namespace mos {

class Texture;
using SharedTexture = std::shared_ptr<Texture>;

/**
 * @brief The Texture2D class
 *
 * Describes a texture in two dimension. Contains iterable chars as data.
 */
class Texture final {
  friend class RenderSystem;

public:
  /**
   * @brief Container for pixel data. RGB(A)
   */
  using Texels = std::vector<unsigned char>;

  enum class Wrap { REPEAT, CLAMP };

  template <class T>
  /**
   * @brief Texture2D
   * @param begin
   * @param end
   * @param width
   * @param height
   * @param mipmaps Generate mipmaps or not.
   *
   * Constructor for a texture, that takes char valus from a container as input.
   *Along with width and height.
   */
  Texture(T begin, T end, unsigned int width, unsigned int height,
          const bool mipmaps = true, const bool compress = true,
          const Wrap &wrap = Wrap::REPEAT)
      : mipmaps(mipmaps), compress(compress), width_(width), height_(height),
        id_(current_id_++), texels_(begin, end), wrap(wrap) {}

  /**
   * @brief Texture2D constructor.
   * @param width
   * @param height
   * @param mipmaps Generate mipmaps or not.
   */
  Texture(const unsigned int width, const unsigned int height,
          const bool mipmaps = true, const bool compress = true,
          const Wrap &wrap = Wrap::REPEAT);

  /**
   * @brief Destructor.
   */
  virtual ~Texture();

  static SharedTexture load(const std::string &path, const bool mipmaps = true,
                            const bool compress = true,
                            const Texture::Wrap &wrap = Texture::Wrap::REPEAT);

  Texture(const std::string &path, const bool mipmaps = true,
          const bool compress = true,
          const Texture::Wrap &wrap = Texture::Wrap::REPEAT);

  /**
   * @brief begin iterator
   * @return constand begin iterator
   */
  Texels::const_iterator begin() const;

  /**
   * @brief end iterator
   * @return constant end iterator
   */
  Texels::const_iterator end() const;

  /**
   * @brief unique id
   * @return id
   */
  unsigned int id() const;

  /**
   * @brief width in pixels
   * @return
   */
  unsigned int width() const;

  /**
   * @brief height in pixels
   * @return
   */
  unsigned int height() const;

  /**
   * @brief size of buffer
   * @return
   */
  unsigned int size() const;

  /**
   * @brief sample the texture
   * @param x less than width
   * @param y less than height
   * @return
   */
  glm::vec4 sample(const unsigned int x, const unsigned int y);

  /**
   * @brief data
   * @return Raw bytes.
   */
  const unsigned char *data() const;

  /**
   * @brief if mipmaps should be used
   */
  const bool mipmaps;
  /**
   * @brief compress
   */
  const bool compress;
  /**
   * @brief wrap
   */
  Wrap wrap;

private:
  static std::atomic_uint current_id_;
  unsigned int id_;
  unsigned int width_;
  unsigned int height_;
  Texels texels_;
};
}

#endif /* MOS_TEXTURE_H */
