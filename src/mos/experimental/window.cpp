#include <iostream>
#include <mos/experimental/window.hpp>

namespace mos {

Window::ErrorFunc Window::error_func;
Window::PosFunc Window::pos_func;
Window::SizeFunc Window::size_func;
Window::ClickFunc Window::click_func;
Window::MouseFunc Window::mouse_func;
Window::ScrollFunc Window::scroll_func;
Window::KeyFunc Window::key_func;

Window::Window(const std::string &title, const glm::ivec2 &resolution) {
  if (!glfwInit()) {
    exit(EXIT_FAILURE);
  }
  glfwSetErrorCallback(error_callback);

  glfwWindowHint(GLFW_SAMPLES, 8);
  glfwWindowHint(GLFW_RED_BITS, 8);
  glfwWindowHint(GLFW_GREEN_BITS, 8);
  glfwWindowHint(GLFW_BLUE_BITS, 8);
  glfwWindowHint(GLFW_ALPHA_BITS, 8);
  glfwWindowHint(GLFW_SRGB_CAPABLE, true);
  glfwWindowHint(GLFW_RESIZABLE, false);

  // Context
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  window_ = glfwCreateWindow(resolution.x, resolution.y, title.c_str(), nullptr,
                             nullptr);
  glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  GLFWcursor *hand_cursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
  GLFWcursor *arrow_cursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);

  if (!window_) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window_);

  glfwSetKeyCallback(window_, key_callback);
  glfwSetWindowSizeCallback(window_, size_callback);
  glfwSetMouseButtonCallback(window_, click_callback);
  glfwSetCursorPosCallback(window_, mouse_callback);
  glfwSetScrollCallback(window_, scroll_callback);
  glfwSetWindowPosCallback(window_, position_callback);

  glfwSwapInterval(1);

  error_func = [](int error, const std::string &description) {
    fputs(description.c_str(), stderr);
  };
}

Window::~Window() { glfwDestroyWindow(window_); }

void Window::error_callback(int error, const char *description) {
  if (error_func) {
    error_func(error, std::string(description));
  }
}

void Window::position_callback(GLFWwindow *window, int x, int y) {
  if (pos_func) {
    pos_func(glm::ivec2(x, y));
  }
}

void Window::size_callback(GLFWwindow *window, int width, int height) {
  if (size_func) {
    size_func(glm::ivec2(width, height));
  }
}

void Window::click_callback(GLFWwindow *window, int button, int action,
                            int mods) {
  if (click_func) {
    click_func(button, action, mods);
  }
}

void Window::mouse_callback(GLFWwindow *window, double x, double y) {
  if (mouse_func) {
    mouse_func(glm::dvec2(x, y));
  }
}

void Window::scroll_callback(GLFWwindow *window, double xoffset,
                             double yoffset) {
  if (scroll_func) {
    scroll_func(glm::dvec2(xoffset, yoffset));
  }
}
void Window::key_callback(GLFWwindow *window, int key, int scancode, int action,
                          int mods) {
  if (key_func) {
    key_func(key, scancode, action, mods);
  }
}

glm::ivec2 Window::framebuffer_size() const {
  int width, height;
  glfwGetFramebufferSize(window_, &width, &height);
  return glm::ivec2(width, height);
}

void Window::poll_events() { glfwPollEvents(); }

void Window::swap_buffers() { glfwSwapBuffers(window_); }

bool Window::close() const { return glfwWindowShouldClose(window_); }
void Window::close(const bool close) {
  glfwSetWindowShouldClose(window_, close);
}
}
