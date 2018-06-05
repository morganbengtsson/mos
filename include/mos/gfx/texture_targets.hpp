#pragma once

#include <initializer_list>
#include <vector>
#include <mos/gfx/texture_target.hpp>

namespace mos {
namespace gfx {

class TextureTargets {
public:
  using Container = std::vector<TextureTarget>;
  template<class It>
  TextureTargets(It begin, It end) : texture_targets_(begin, end){};
  TextureTargets(const std::initializer_list<TextureTargets> &);
  TextureTargets();
  void push_back(const TextureTarget& texture_target);
  Container::const_iterator begin() const;
  Container::const_iterator end() const;
  Container::iterator begin();
  Container::iterator end();
  TextureTarget& operator[](unsigned long pos);
private:
  Container texture_targets_;
};
}
}

