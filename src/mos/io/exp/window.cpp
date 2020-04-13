#include <mos/io/exp/window.hpp>

namespace mos::io::exp {

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

Window::Output Window::poll_events() {
  keyboard_.events.clear();
  keyboard_.codepoints.clear();
  mouse_.events.clear();

  glfwPollEvents();

  Output output{keyboard_, mouse_};
  return output;
}

void Window::error_callback(int error, const char *description) {
  fprintf(stderr, "Error: %s\n", description);
}

void Window::key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ENTER) {
    if (action == GLFW_PRESS) {
      keyboard_.events.insert(mos::io::Keyboard::Event::Button_enter_press);
    }
    else if (action == GLFW_RELEASE){
      keyboard_.events.insert(mos::io::Keyboard::Event::Button_enter_release);
    }
  }

  if (key == GLFW_KEY_BACKSPACE) {
    if (action == GLFW_PRESS) {
      keyboard_.events.insert(mos::io::Keyboard::Event::Button_backspace_press);
    }
    else if (action == GLFW_RELEASE){
      keyboard_.events.insert(mos::io::Keyboard::Event::Button_backspace_release);
    }
    }
}

void Window::char_callback(GLFWwindow * window, const unsigned int codepoint) {
  keyboard_.codepoints.push_back(codepoint);
}

void Window::cursor_position_callback(GLFWwindow * window, double xpos, double ypos) {
  mouse_.old_position = mouse_.position;
  mouse_.position = glm::dvec2(xpos, ypos);
}

void Window::mouse_button_callback(GLFWwindow * window, int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) {
    mouse_.buttons[0] = true;
    mouse_.events.insert(mos::io::Mouse::Event::Button_press_0);
  }
  if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) {
    mouse_.buttons[0] = false;
    mouse_.events.insert(mos::io::Mouse::Event::Button_release_0);
  }
  if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS) {
    mouse_.buttons[1] = true;
    mouse_.events.insert(mos::io::Mouse::Event::Button_press_1);
  }
  if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_RELEASE) {
    mouse_.buttons[1] = false;
    mouse_.events.insert(mos::io::Mouse::Event::Button_release_1);
  }

  if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
    mouse_.buttons[2] = true;
  }
  if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE) {
    mouse_.buttons[2] = false;
  }
}
}

