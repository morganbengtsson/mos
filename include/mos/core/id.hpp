#pragma once

#include <atomic>

namespace mos {

template<class T>
class Id {
public:
  Id() : id(++current_id){}
  operator int&() { return id; }
  operator int() const { return id; }
private:
  int id;
  static std::atomic<int> current_id;
};

template<class T> std::atomic<int> Id<T>::current_id = 0;

} // namespace mos
