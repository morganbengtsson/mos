#pragma once

#include <chrono>
#include <glad/glad.h>

class Buffer {
public:
  using Time_point = std::chrono::time_point<std::chrono::system_clock,
                                             std::chrono::nanoseconds>;

  Buffer(GLenum type, GLsizeiptr size, const void *data, GLenum hint,
         Time_point modified);
  Buffer(const Buffer &buffer) = delete;
  Buffer(Buffer &&buffer) noexcept;
  Buffer &operator=(const Buffer &buffer) = delete;
  Buffer &operator=(Buffer &&buffer) noexcept;
  ~Buffer();

  GLuint id{0};
  Time_point modified;

private:
  void release();
};
