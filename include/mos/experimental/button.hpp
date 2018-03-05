#ifndef MOS_BUTTON_HPP
#define MOS_BUTTON_HPP

#include <functional>
#include <mos/render/model.hpp>
#include <mos/render/text.hpp>

namespace mos {
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
  mos::gfx::Material light_material_;
  mos::gfx::Material dark_material_;
  std::function<void()> click_callback_;
};
}
#endif // MOS_BUTTON_HPP
