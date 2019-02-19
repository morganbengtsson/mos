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

  glm::dvec2 position;
  //TODO: set automaticly
  glm::dvec2 old_position;
  Events events;
  std::array<bool, 3> buttons;
};

}
}
