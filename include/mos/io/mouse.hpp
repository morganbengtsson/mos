#pragma once

#include <glm/glm.hpp>
#include <set>
#include <array>

namespace mos {
namespace io {

class Mouse {
public:
  Mouse();
  enum class Event {
    Button_press_0,
    Button_release_0,
  };
  using Events = std::set<Event>;

  glm::vec2 position;
  Events events;
  std::array<bool, 2> buttons;
};

}
}
