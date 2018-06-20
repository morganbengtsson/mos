#include <functional>
#pragma once

namespace mos {
namespace exp {

template<typename T>
/** Triggers when the value changes */
class Trigger {
public:
  Trigger(const T& value):
      value_(value),
      old_value_(value){}
  T& operator=(const T& value){
    old_value_ = value_;
    value_ = value;
  }
  bool changed() const{ return old_value_ != value_;};
  operator T() const{
    return value_;
  }
private:
  T value_;
  T old_value_;
};

}
}
