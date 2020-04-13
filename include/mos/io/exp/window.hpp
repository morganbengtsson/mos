#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <glm/glm.hpp>
#include <mos/io/keyboard.hpp>
#include <mos/io/mouse.hpp>

namespace mos::io::exp {

class Window {
public:
  struct Output {
    Keyboard keyboard;
    Mouse mouse;
  };
  Window(const std::string &title = "Window",
        const glm::ivec2 &resolution = {1920, 1080},
         int swap_interval = 0);
  ~Window();
  Output poll_events();
  void swap_buffers();
private:
  GLFWwindow *window_;
  static Keyboard keyboard_;
  static Mouse mouse_;
  static void error_callback(int error, const char* description);
  static void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods);
  static void char_callback(GLFWwindow * window, const unsigned int codepoint);
  static void cursor_position_callback(GLFWwindow * window, double xpos, double ypos);
  static void mouse_button_callback(GLFWwindow * window, int button, int action, int mods);
};
}
