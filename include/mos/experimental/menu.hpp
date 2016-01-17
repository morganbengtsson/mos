#ifndef MENU_HPP
#define MENU_HPP

#include "button.hpp"
#include <initializer_list>
#include <vector>
#include <algorithm>

namespace mos {
class Menu {
public:
  using Buttons = std::vector<Button>;
  Menu();
  Menu(const std::initializer_list<Button> &buttons);
  void add(const Button &button);
  mos::Model model();
  Button &selected();
  void select_next();
  void select_previos();
  ~Menu();

private:
  Buttons buttons_;
  template <class T> void select_next(T begin, T end) {
    auto it = std::find_if(
        begin, end, [](mos::Button &button) { return button.selected(); });
    if ((it != end) && (std::next(it) == end)) {
      for (auto &button : buttons_) {
        button.state(Button::State::IDLE);
      }
      begin->state(Button::State::SELECTED);
    } else {
      it->state(Button::State::IDLE);
      it++;
      it->state(Button::State::SELECTED);
    }
  }
};
}
#endif // MENU_HPP
