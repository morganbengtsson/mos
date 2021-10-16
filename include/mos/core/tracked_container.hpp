#pragma once
#include <chrono>
#include <initializer_list>
#include <mos/core/container.hpp>
#include <vector>

namespace mos {

template <class T> class Container;

/** Container with modified time stamp. */
template <class T> class Tracked_container {
public:
  using Items = std::vector<T>;
  using TimePoint = std::chrono::time_point<std::chrono::system_clock,
                                            std::chrono::nanoseconds>;
  using size_type = typename Items::size_type;

  Tracked_container() { invalidate(); }

  template <class It>
  explicit Tracked_container(const std::initializer_list<It> list)
      : Tracked_container(list.begin(), list.end()) {}

  template <class It> Tracked_container(It begin, It end) {
    assign(begin, end);
  }

  explicit Tracked_container(const Container<T> &container)
      : Tracked_container(container.begin(), container.end()) {}

  template <class It> void assign(It begin, It end) {
    items_.assign(begin, end);
    invalidate();
  }
  typename Items::iterator begin() {
    invalidate();
    return items_.begin();
  }
  typename Items::iterator end() {
    invalidate();
    return items_.end();
  }
  typename Items::const_iterator begin() const { return items_.begin(); }
  typename Items::const_iterator end() const { return items_.end(); }
  typename Items::reference operator[](typename Items::size_type pos) {
    invalidate();
    return items_[pos];
  }
  typename Items::const_reference
  operator[](typename Items::size_type pos) const {
    return items_[pos];
  }
  typename Items::size_type size() const { return items_.size(); }
  typename Items::reference back() {
    invalidate();
    return items_.back();
  }
  const T *data() const noexcept { return items_.data(); }
  void clear() {
    items_.clear();
    invalidate();
  }
  void push_back(const T &item) {
    items_.push_back(item);
    invalidate();
  }

  TimePoint modified() const { return modified_; }

private:
  void invalidate() { modified_ = std::chrono::system_clock::now(); }
  Items items_;
  TimePoint modified_;
};
} // namespace mos
