#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <functional>
#include <glm/glm.hpp>
#include <string>

namespace mos {
namespace io {

class Window {
public:
  enum class Cursor_mode : int {
    Normal = GLFW_CURSOR_NORMAL,
    Hidden = GLFW_CURSOR_HIDDEN,
    Disabled = GLFW_CURSOR_DISABLED
  };

  enum class Cursor {
    Hand, Arrow, Crosshair
  };

  Window(const std::string &title = "Window", const glm::ivec2 &resolution = {1920, 1080}, int swap_interval = 0);
  ~Window();
  Window(const Window & window) = delete;
  Window(const Window && window) = delete;
  Window & operator=(const Window & window) = delete;
  Window & operator=(const Window && window) = delete;

  using Error_func =
  std::function<void(const int error, const std::string &description)>;
  using Pos_func = std::function<void(const glm::vec2 &position)>;
  using Size_func = std::function<void(const glm::vec2 &size)>;
  using Click_func =
  std::function<void(const int button, const int action, const int mods)>;
  using Mouse_func = std::function<void(const glm::dvec2 &position)>;
  using Scroll_func = std::function<void(const glm::dvec2 &offset)>;
  using Key_func = std::function<void(const int key, const int scancode,
                                     const int action, const int mods)>;
  using Char_func = std::function<void(const unsigned int codepoint)>;

  static Error_func error_func;
  static Pos_func pos_func;
  static Size_func size_func;
  static Click_func click_func;
  static Mouse_func mouse_func;
  static Scroll_func scroll_func;
  static Key_func key_func;
  static Char_func char_func;

  glm::ivec2 framebuffer_size() const;
  void poll_events();
  void swap_buffers();
  bool close() const;
  void close(bool close);
  void cursor_mode(const Cursor_mode &mode);
  void cursor(const Cursor &cursor);
  glm::dvec2 cursor_position() const;
  float dpi() const;

private:
  GLFWwindow *window_;
  GLFWcursor *hand_cursor_;
  GLFWcursor *arrow_cursor_;
  GLFWcursor *crosshair_cursor_;

  static void error_callback(int error, const char *description);
  static void position_callback(GLFWwindow *window, int x, int y);
  static void size_callback(GLFWwindow *window, int width, int height);
  static void click_callback(GLFWwindow *window, int button, int action,
                             int mods);
  static void mouse_callback(GLFWwindow *window, double x, double y);
  static void scroll_callback(GLFWwindow *window, double xoffset,
                              double yoffset);
  static void key_callback(GLFWwindow *window, int key, int scancode,
                           int action, int mods);
  static void char_callback(GLFWwindow * window, unsigned int codepoint);

  bool pause_ = false;
  float pause_time_ = 0.0f;
  const float max_pause_ = 0.2f;
};
}
}

