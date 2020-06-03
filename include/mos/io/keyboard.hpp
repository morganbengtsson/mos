#pragma once

#include <unordered_set>
#include <vector>

namespace mos::io {

class Keyboard {
public:
  Keyboard() = default;

  enum class Key : int {
    Enter = 257,
    Escape = 256,
    Backspace = 259,
    F1 = 290,
    F2 = 291,
    F3 = 292,
    F4 = 293,
    F5 = 294,
    F6 = 295,
    F7 = 296,
    F8 = 297,
    F9 = 298,
    F10 = 299,
    F11 = 300,
    F12 = 301
  };

  enum class Action : int {
    Release = 0,
    Press = 1,
    Repeat = 2
  };

  struct Event {
    Key key;
    Action action;

    bool operator==(const Event &event) const{
      return key == event.key && action == event.action;
    }
  };

private:
  struct EventHash {
    size_t operator()(const Event& e) const {
      return ((std::hash<int>()(static_cast<int>(e.key)) ^ (std::hash<int>()(static_cast<int>(e.action)) << 1)) >> 1);
    }
  };
public:
  using Events = std::unordered_set<Event, EventHash>;
  using Codepoints = std::vector<unsigned int>;
  Events events;
  Codepoints codepoints;
};
}
