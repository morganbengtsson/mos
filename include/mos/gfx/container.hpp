#pragma once

#include <initializer_list>
#include <vector>
#include <mos/gfx/texture_target.hpp>

namespace mos {
namespace gfx {

template <class T>
class Container {
public:
  using Items = std::vector<T>;
  template<class It>
  Container(It begin, It end) : items_(begin, end){};
  Container(const std::initializer_list<T> &list):items_(list.begin(), list.end()){};
  Container(){};
  typename Items::const_iterator begin() const{
    return items_.begin();
  };
  typename Items::const_iterator end() const{
    return items_.end();
  }
  typename Items::const_reference operator[](unsigned long pos){
    return items_[pos];
  }
private:
  Items items_;
};
}
}

