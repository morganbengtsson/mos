#pragma once

#include <glad/glad.h>
#include <mos/gfx/texture_2d.hpp>

class Texture_buffer_2D {
public:
  using Time_point =  std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>;

  explicit Texture_buffer_2D(const Texture_2D &texture_2d);
  Texture_buffer_2D(
      GLint internal_format, GLenum external_format, int width, int height,
      GLint filter_min, GLint filter_mag, GLint wrap, const void *data,
      const Time_point &modified = std::chrono::system_clock::now());

 ~Texture_buffer_2D();
  Texture_buffer_2D(const Texture_buffer_2D &buffer) = delete;
  Texture_buffer_2D(Texture_buffer_2D &&buffer) noexcept;
  Texture_buffer_2D & operator=(const Texture_buffer_2D &buffer) = delete;
  Texture_buffer_2D & operator=(Texture_buffer_2D &&buffer) noexcept;
  GLuint texture{0};
  Time_point modified;
private:
  void release();
};
