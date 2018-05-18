#include <functional>
#pragma once

namespace mos {
namespace exp {

template<typename T>
/** Triggers when the value changes */
class Trigger {
public:
  Trigger(const T& value, const std::function<void()>& on_change):
      value_(value),
      old_value_(value),
      on_change_(on_change){}
  T& operator=(const T& value){
    old_value_ = value_;
    value_ = value;
    if(old_value_ != value_){
      on_change_();
    }
  }
  bool changed() const{ return old_value_ != value_;};
  operator T(){
    return value_;
  }
private:
  T value_;
  T old_value_;
  std::function<void()> on_change_;
};

}
}
