#pragma once

#include <glm/glm.hpp>
#include <vector>
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
  using Events = std::vector<Event>;

  glm::vec2 position;
  Events events;
  std::array<bool, 2> buttons;
};

}
}
