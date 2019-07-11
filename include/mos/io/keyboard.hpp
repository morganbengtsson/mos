#pragma once

#include <set>
#include <vector>

namespace mos::io {
class Keyboard {
public:
  Keyboard() = default;
  enum class Event{
    Button_enter_press,
    Button_enter_release,
    Button_backspace_press,
    Button_backspace_release,
  };
  using Events = std::set<Event>;
  using Codepoints = std::vector<unsigned int>;
  Events events;
  Codepoints codepoints;
};
}
