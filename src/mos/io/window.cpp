#include <mos/io/window.hpp>

namespace mos::io {

Keyboard Window::keyboard_ = Keyboard();
Mouse Window::mouse_ = Mouse();

Window::Window(const std::string &title,
               const glm::ivec2 &resolution,
               int swap_interval){
  if (!glfwInit()) {
    exit(EXIT_FAILURE);
  }

  glfwWindowHint(GLFW_SRGB_CAPABLE, true);
  glfwWindowHint(GLFW_RESIZABLE, false);

  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

  window_ = glfwCreateWindow(resolution.x, resolution.y, title.c_str(), nullptr,
                             nullptr);
  glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

  if (!window_) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window_);

  gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

  glfwSwapInterval(swap_interval);

  glfwSetErrorCallback(error_callback);
  glfwSetKeyCallback(window_, key_callback);
  glfwSetCharCallback(window_, char_callback);
  glfwSetCursorPosCallback(window_, cursor_position_callback);
  glfwSetMouseButtonCallback(window_, mouse_button_callback);
}

Window::~Window(){
  glfwDestroyWindow(window_);
}

void Window::swap_buffers() {
  glfwSwapBuffers(window_);
}

Window::Output Window::poll_events() {
  keyboard_.events.clear();
  keyboard_.codepoints.clear();
  mouse_.events.clear();

  glfwPollEvents();

  return Output{keyboard_, mouse_};
}

float Window::dpi() const {
  GLFWmonitor *monitor = glfwGetPrimaryMonitor();
  int widthMM, heightMM;
  glfwGetMonitorPhysicalSize(monitor, &widthMM, &heightMM);
  const GLFWvidmode *mode = glfwGetVideoMode(monitor);
  const double dpi = mode->width / (widthMM / 25.4);
  return dpi;
}

bool Window::close() const {
  return glfwWindowShouldClose(window_);
}

void Window::close(const bool close) {
  glfwSetWindowShouldClose(window_, close);
}

void Window::error_callback(int error, const char *description) {
  fprintf(stderr, "Error: %s\n", description);
}

void Window::key_callback(GLFWwindow * window, int key, int scancode, int action, int mods) {
  keyboard_.events.insert({static_cast<Keyboard::Key>(key), static_cast<Keyboard::Action>(action)});
}

void Window::char_callback(GLFWwindow * window, const unsigned int codepoint) {
  keyboard_.codepoints.push_back(codepoint);
}

void Window::cursor_position_callback(GLFWwindow * window, double xpos, double ypos) {
  mouse_.old_position = mouse_.position;
  mouse_.position = glm::dvec2(xpos, ypos);
}

void Window::mouse_button_callback(GLFWwindow * window, int button, int action, int mods) {
  mouse_.buttons[button] = static_cast<bool>(action);
  mouse_.events.insert({static_cast<Mouse::Button>(button), static_cast<Mouse::Action>(action)});
}
}

