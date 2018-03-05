#pragma once

#include <atomic>
#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <mos/gfx/texture.hpp>

namespace mos {
namespace gfx {

class Texture2D;
using SharedTexture2D = std::shared_ptr<Texture2D>;
/**
 * @brief The Texture2D class
 *
 * Describes a texture in two dimension. Contains iterable chars as data.
 */
class Texture2D final : public Texture {
  friend class Renderer;
public:
  template<class T>
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
  Texture2D(T begin, T end, unsigned int width, unsigned int height,
            const Format &format = Format::SRGBA, const Wrap &wrap = Wrap::REPEAT,
            const bool mipmaps = true)
      : Texture({Data(begin, end)}, width, height, format, wrap, mipmaps) {}

  /**
   * @brief Texture2D constructor.
   * @param width
   * @param height
   * @param mipmaps Generate mipmaps or not.
   */
  Texture2D(const unsigned int width, const unsigned int height,
            const Format &format = Format::RGBA, const Wrap &wrap = Wrap::REPEAT,
            const bool mipmaps = true);

  /**
   * @brief Destructor.
   */
  virtual ~Texture2D();

  static SharedTexture2D load(const std::string &path,
                              const bool mipmaps = true,
                              const Texture2D::Wrap &wrap = Texture2D::Wrap::REPEAT);

  Texture2D(const std::string &path,
            const bool mipmaps = true,
            const Texture2D::Wrap &wrap = Texture2D::Wrap::REPEAT);
};
}
}
