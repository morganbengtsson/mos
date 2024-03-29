#pragma once

#include <mos/gl/render_buffer.hpp>
#include <mos/gl/texture_buffer_2d.hpp>
#include <mos/gfx/texture_target.hpp>

namespace mos::gl {

class Frame_buffer {
  friend class Renderer;
private:
  explicit Frame_buffer(const gfx::Texture_target &target,
                        Texture_buffers &texture_buffers,
                        Render_buffers &render_buffers);
public:
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
