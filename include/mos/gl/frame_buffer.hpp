#pragma once

#include <mos/gl/render_buffer.hpp>
#include <mos/gl/texture_buffer_2d.hpp>
#include <mos/gfx/texture_target.hpp>

namespace mos::gfx::gl {

class Frame_buffer {
public:
  explicit Frame_buffer(const Texture_target &target,
                        Texture_buffers &texture_buffers,
                        Render_buffers &render_buffers);
  ~Frame_buffer();
  Frame_buffer(Frame_buffer &&buffer) noexcept;
  Frame_buffer(const Frame_buffer &buffer) = delete;
  Frame_buffer &operator=(const Frame_buffer &buffer) = delete;
  Frame_buffer &operator=(Frame_buffer &&buffer) noexcept;
  GLuint id{0};

private:
  void release();
};
}
