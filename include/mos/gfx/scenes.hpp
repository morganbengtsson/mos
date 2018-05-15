#pragma once

#include <mos/gfx/scene.hpp>
#include <initializer_list>

namespace mos{
namespace gfx{

class Scenes {
public:
  using Container = std::vector<Scene>;
  template<class It>
  Scenes(It begin, It end) : scenes_(begin, end){};
  Scenes(const std::initializer_list<Scene> &scenes);
  Scenes();
  void push_back(const Scene& scene);
  Container::const_iterator begin() const;
  Container::const_iterator end() const;
private:
  std::vector<Scene> scenes_;
};

}
}