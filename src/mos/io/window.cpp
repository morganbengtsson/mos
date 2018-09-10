#include <iostream>
#include <mos/io/window.hpp>

namespace mos {
namespace io {

Window::Error_func Window::error_func;
Window::Pos_func Window::pos_func;
Window::Size_func Window::size_func;
Window::Click_func Window::click_func;
Window::Mouse_func Window::mouse_func;
Window::Scroll_func Window::scroll_func;
Window::Key_func Window::key_func;

Window::Window(const std::string &title, const glm::ivec2 &resolution, const int swap_interval) {
  if (!glfwInit()) {
    exit(EXIT_FAILURE);
  }
  glfwSetErrorCallback(error_callback);

  //glfwWindowHint(GLFW_SAMPLES, 8);
  glfwWindowHint(GLFW_SRGB_CAPABLE, true);
  glfwWindowHint(GLFW_RESIZABLE, false);

  // Context
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

  window_ = glfwCreateWindow(resolution.x, resolution.y, title.c_str(), nullptr,
                             nullptr);
  glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

  // glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  if (!window_) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window_);

  gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

  glfwSetKeyCallback(window_, key_callback);
  glfwSetWindowSizeCallback(window_, size_callback);
  glfwSetMouseButtonCallback(window_, click_callback);
  glfwSetCursorPosCallback(window_, mouse_callback);
  glfwSetScrollCallback(window_, scroll_callback);
  glfwSetWindowPosCallback(window_, position_callback);

  glfwSwapInterval(swap_interval);

  hand_cursor_ = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
  arrow_cursor_ = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
  crosshair_cursor_ = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);

  error_func = [](int error, const std::string &description) {
    fputs(description.c_str(), stderr);
  };
}

Window::~Window() {
  glfwDestroyCursor(hand_cursor_);
  glfwDestroyCursor(arrow_cursor_);
  glfwDestroyWindow(window_);
}

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

void Window::cursor_mode(const Window::Cursor_mode &mode) {
  glfwSetInputMode(window_, GLFW_CURSOR, static_cast<int>(mode));
}

void Window::cursor(const Window::Cursor &cursor) {
  if (cursor == Cursor::Hand) {
    glfwSetCursor(window_, hand_cursor_);
  } else if (cursor == Cursor::Arrow){
    glfwSetCursor(window_, arrow_cursor_);
  }
  else if (cursor == Cursor::Crosshair){
    glfwSetCursor(window_, crosshair_cursor_);
  }
}

glm::dvec2 Window::cursor_position() const {
  double xpos, ypos;
  glfwGetCursorPos(window_, &xpos, &ypos);
  return glm::dvec2(xpos, ypos);
}

float Window::dpi() const {
  GLFWmonitor *monitor = glfwGetPrimaryMonitor();
  int widthMM, heightMM;
  glfwGetMonitorPhysicalSize(monitor, &widthMM, &heightMM);
  const GLFWvidmode *mode = glfwGetVideoMode(monitor);
  const double dpi = mode->width / (widthMM / 25.4);
  return dpi;
}
}
}
