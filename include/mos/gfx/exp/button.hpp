#pragma once

#include <functional>
#include <mos/gfx/model.hpp>
#include <mos/gfx/text.hpp>
#include <mos/gfx/material.hpp>

namespace mos {
namespace gfx {
namespace exp {

class Button {
public:
  using Callback = std::function<void()>;
  enum class State { IDLE, SELECTED, CLICKED };
  Button(const gfx::Text &text, const State &state = State::IDLE);
  ~Button();
  gfx::Model model();
  bool selected();
  void state(const State &state);
  float height() const;
  float width() const;
  State state() const;
  void click_callback(const Callback &callback);
  void click();

private:
  gfx::Text text_;
  gfx::Model rectangle_;
  float padding_;
  State state_;
  Material light_material_;
  Material dark_material_;
  std::function<void()> click_callback_;
};
}
}
}