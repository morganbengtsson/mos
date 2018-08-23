#pragma once
#include <vector>
#include <chrono>
#include <initializer_list>
#include <mos/core/container.hpp>

namespace mos {

template<class T> class Container;

/** Container with modified time stamp. */
template<class T>
class TrackedContainer {
public:
  using Items = std::vector<T>;
  using TimePoint = std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>;

  TrackedContainer(){
    invalidate();
  };

  template<class It>
  TrackedContainer(const std::initializer_list<It> list): TrackedContainer(list.begin(), list.end()){}

  template<class It>
  TrackedContainer(It begin, It end) {
    assign(begin, end);
  }

  TrackedContainer(const Container<T> &container): TrackedContainer(container.begin(), container.end()){}

  template<class It>
  void assign(It begin, It end){
    items_.assign(begin, end);
    invalidate();
  }
  typename Items::iterator begin() {
    return items_.begin();
  }
  typename Items::iterator end() {
    invalidate();
    return items_.end();
  }
  typename Items::const_iterator begin() const {
    return items_.begin();
  }
  typename Items::const_iterator end() const {
    return items_.end();
  }
  typename Items::reference operator[](typename Items::size_type pos){
    invalidate();
    return items_[pos];
  }
  typename Items::const_reference operator[](typename Items::size_type pos) const {
    return items_[pos];
  }
  typename Items::size_type size() const {
    return items_.size();
  }
  typename Items::reference back() {
    invalidate();
    return items_.back();
  }
  const T * data() const noexcept {
    return items_.data();
  }
  void clear(){
    items_.clear();
    invalidate();
  }
  void push_back(const T &item){
    items_.push_back(item);
    invalidate();
  }

  TimePoint modified() const {
    return modified_;
  }

private:
  void invalidate() {
    modified_ = std::chrono::system_clock::now();
  }
  Items items_;
  TimePoint modified_;
};
}