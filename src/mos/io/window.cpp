#include <mos/io/window.hpp>
#include <spdlog/spdlog.h>

namespace mos::io {

Keyboard Window::keyboard_ = Keyboard();
Mouse Window::mouse_ = Mouse();
Gamepad Window::gamepad_ = Gamepad();

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

  if (window_ == nullptr) {
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

auto Window::poll_events() -> Output {
  keyboard_.events.clear();
  keyboard_.codepoints.clear();
  mouse_.events.clear();

  glfwPollEvents();

  GLFWgamepadstate state;
  if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state)) {
    gamepad_.button_a = bool(state.buttons[GLFW_GAMEPAD_BUTTON_A]);
    gamepad_.button_b = bool(state.buttons[GLFW_GAMEPAD_BUTTON_B]);

    gamepad_.left_axis.x = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
    gamepad_.left_axis.y = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];

    gamepad_.right_axis.x = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
    gamepad_.right_axis.y = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];
  }

  return Output{keyboard_, mouse_, gamepad_};
}

auto Window::dpi() const -> float {
  GLFWmonitor *monitor = glfwGetPrimaryMonitor();
  int widthMM{0};
  int heightMM{0};
  glfwGetMonitorPhysicalSize(monitor, &widthMM, &heightMM);
  const GLFWvidmode *mode = glfwGetVideoMode(monitor);
  const float dpi = mode->width / (widthMM / 25.4f);
  return dpi;
}

auto Window::close() const -> bool {
  return glfwWindowShouldClose(window_);
}

void Window::close(const bool close) {
  glfwSetWindowShouldClose(window_, close);
}

void Window::error_callback(int error, const char *description) {
  spdlog::error("Error: {}", description);
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

