#include <mos/gl/renderer.hpp>
#include <mos/gl/gli_converter.hpp>

namespace mos::gl {

Texture_buffer_2D::Texture_buffer_2D(
    const GLint internal_format, const GLenum external_format, const int width,
    const int height, const GLint filter_min, const GLint filter_mag,
    const GLint wrap, const void *data, const Time_point &modified)
    : texture(Renderer::generate(glGenTextures)), modified(modified) {
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_min);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_mag);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

  glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0,
               external_format, GL_UNSIGNED_BYTE, data);
  if (filter_min == GL_LINEAR_MIPMAP_LINEAR ||
      filter_min == GL_NEAREST_MIPMAP_LINEAR) {
    glGenerateMipmap(GL_TEXTURE_2D);
  };
  glBindTexture(GL_TEXTURE_2D, 0);
}

Texture_buffer_2D::~Texture_buffer_2D() { release(); }

Texture_buffer_2D::Texture_buffer_2D(
    Texture_buffer_2D &&buffer) noexcept
    : texture(buffer.texture) {
  buffer.texture = 0;
}

auto Texture_buffer_2D::operator=(
    Texture_buffer_2D &&buffer) noexcept -> Texture_buffer_2D & {
  if (this != &buffer) {
    release();
    std::swap(texture, buffer.texture);
  }
  return *this;
}

void Texture_buffer_2D::release() {
  glDeleteTextures(1, &texture);
  texture = 0;
}


Texture_buffer_2D::Texture_buffer_2D(const gfx::Texture_2D &texture_2d)
    : Texture_buffer_2D(
          gli_converter.translate(texture_2d.format(), texture_2d.swizzles()).Internal,
          gli_converter.translate(texture_2d.format(), texture_2d.swizzles()).External, texture_2d.width(),
          texture_2d.height(),
          texture_2d.generate_mipmaps ? Renderer::filter_convert_mip(texture_2d.filter)
                                      : Renderer::filter_convert(texture_2d.filter),
          Renderer::filter_convert(texture_2d.filter), Renderer::wrap_convert(texture_2d.wrap),
          texture_2d.data()) {}
} // namespace mos::gfx
