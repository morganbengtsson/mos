#pragma once

#include <GLFW/glfw3.h>
#include <string>
#include <mos/io/keyboard.hpp>
#include <mos/io/mouse.hpp>

namespace mos::io::exp{

class Window
{
public:
  struct Output {
    Keyboard keyboard;
  }
  Window(const std::string &title = "Window",
        const glm::ivec2 &resolution = {1920, 1080},
         int swap_interval = 0);
  ~Window();
  Output poll_events();
  void swap_buffers();
private:
  GLFWwindow *window_;
  Keyboard keyboard_;
  Mouse mouse_;
  void error_callback(int error, const char* description);
  void key_callback(int key, int scancode, int action, int mods);
  void char_callback(const unsigned int codepoint);
  void cursor_position_callback(double xpos, double ypos);
  void mouse_button_callback(int button, int action, int mods);
};


}
