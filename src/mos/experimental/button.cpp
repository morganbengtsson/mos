#include <mos/experimental/button.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace mos {

Button::Button(const Text &text, const State &s)
    : text_(text), padding_(text_.height() / 4.0f),
      light_material_(std::make_shared<Material>(
          glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f), 0.8f)),
      dark_material_(std::make_shared<Material>(
          glm::vec3(0.0f), glm::vec3(0.01f), glm::vec3(0.0f), 0.8f)) {
  mos::Vertex v1({0.0f, -1.0f, 0.0f});
  mos::Vertex v2({0.0f, 0.0f, 0.0f});
  mos::Vertex v3({1.0f, 0.0f, 0.0f});
  mos::Vertex v4({1.0f, -1.0f, 0.0f});

  mos::Mesh mesh({v1, v2, v3, v4}, {0, 2, 1, 0, 3, 2});
  rectangle_.mesh = std::make_shared<Mesh>(mesh);
  rectangle_.transform = glm::scale(
      glm::mat4(1.0f), glm::vec3(text_.width() + padding_ * 2.0f,
                                 text_.height() + padding_ * 2.0f, 1.0f));
  rectangle_.transform =
      glm::translate(rectangle_.transform, glm::vec3(0.0f, 0.0f, -0.0f));
  rectangle_.lit = false;
  rectangle_.mesh->invalidate();
  text_.position(glm::vec2(padding_, -padding_));
  state(s);
}

Button::~Button() {}

Model Button::model() {
  mos::Model out;
  out.models.push_back(rectangle_);
  out.models.push_back(text_.model());
  return out;
}

bool Button::selected() { return (state_ == State::SELECTED); }

void Button::state(const Button::State &state) {
  state_ = state;
  if (state_ == State::IDLE) {
    rectangle_.material = dark_material_;
    text_.material(light_material_);
  } else if (state_ == State::CLICKED) {
    rectangle_.material = light_material_;
    text_.material(dark_material_);
  } else if (state_ == State::SELECTED) {
    rectangle_.material = light_material_;
    text_.material(dark_material_);
  }
}

float Button::height() const { return text_.height() + padding_ * 2.0f; }

float Button::width() const { return text_.width() + padding_ * 2.0f; }

Button::State Button::state() const { return state_; }

void Button::click_callback(const Callback &callback) {
  click_callback_ = callback;
}

void Button::click() { click_callback_(); }
}
