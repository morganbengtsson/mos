#pragma once
#include <mos/gfx/vertex.hpp>
#include <vector>
#include <chrono>
#include <initializer_list>

namespace mos {
namespace gfx {

/** Container with modified time stamp. */
template<class T>
class Container {
public:
  using Cont = std::vector<T>;
  using TimePoint = std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>;

  Container(){
    invalidate();
  };

  template<class It>
  Container(const std::initializer_list<It> list): Container(list.begin(), list.end()){}

  template<class It>
  Container(It begin, It end) {
    assign(begin, end);
  }
  template<class It>

  void assign(It begin, It end){
    data_.assign(begin, end);
    invalidate();
  }
  typename Cont::iterator begin() {
    return data_.begin();
  }
  typename Cont::iterator end() {
    invalidate();
    return data_.end();
  }
  typename Cont::const_iterator begin() const {
    return data_.begin();
  }
  typename Cont::const_iterator end() const {
    return data_.end();
  }
  typename Cont::reference operator[](typename Cont::size_type pos){
    invalidate();
    return data_[pos];
  }
  typename Cont::const_reference operator[](typename Cont::size_type pos) const {
    return data_[pos];
  }
  typename Cont::size_type size() const {
    return data_.size();
  }
  typename Cont::reference back() {
    invalidate();
    return data_.back();
  }
  const T * data() const noexcept {
    return data_.data();
  }
  void clear(){
    data_.clear();
    invalidate();
  }
  void push_back(const T &vertex){
    data_.push_back(vertex);
    invalidate();
  }

  TimePoint modified() const {
    return modified_;
  }

private:
  void invalidate() {
    modified_ = std::chrono::system_clock::now();
  }
  std::vector<T> data_;
  TimePoint modified_;
};

}
}