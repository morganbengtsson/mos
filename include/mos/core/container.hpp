#pragma once
#include <vector>
#include <chrono>
#include <initializer_list>
#include <mos/core/tracked_container.hpp>

namespace mos {

template<class T> class Tracked_container;

/** Container. */
template<class T>
class Container {
public:
  using Items = std::vector<T>;

  Container() = default;

  template<class It>
  Container(const std::initializer_list<It> list): Container(list.begin(), list.end()){}

  template<class It>
  Container(It begin, It end) {
    assign(begin, end);
  }

  Container(const Tracked_container<T> & container) : Container(container.begin(), container.end()){}

  template<class It>
  void assign(It begin, It end){
    items_.assign(begin, end);
  }

  template<class It>
  typename Items::iterator insert(typename Items::const_iterator pos, It begin, It end){
    return items_.insert(pos, begin, end);
  }

  typename Items::iterator begin() {
    return items_.begin();
  }
  typename Items::iterator end() {
    return items_.end();
  }
  typename Items::const_iterator begin() const {
    return items_.begin();
  }
  typename Items::const_iterator end() const {
    return items_.end();
  }
  void assign(const std::initializer_list<T>& list){
    items_.assign(list);
  }
  typename Items::reference operator[](typename Items::size_type pos){
    return items_[pos];
  }
  typename Items::const_reference operator[](typename Items::size_type pos) const {
    return items_[pos];
  }
  typename Items::size_type size() const {
    return items_.size();
  }
  typename Items::reference back() {
    return items_.back();
  }
  const T * data() const noexcept {
    return items_.data();
  }
  void clear() {
    items_.clear();
  }

  void push_back(const T &item){
    items_.push_back(item);
  }

  void append(const Container<T> &container) {
    insert(end(), container.begin(), container.end());
  }

private:
  Items items_;
};

}