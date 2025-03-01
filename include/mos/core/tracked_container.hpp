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
  auto begin() -> typename Items::iterator {
    invalidate();
    return items_.begin();
  }
  auto end() -> typename Items::iterator {
    invalidate();
    return items_.end();
  }
  auto begin() const -> typename Items::const_iterator { return items_.begin(); }
  auto end() const -> typename Items::const_iterator { return items_.end(); }
  auto operator[](typename Items::size_type pos) -> typename Items::reference {
    invalidate();
    return items_[pos];
  }
  auto
  operator[](typename Items::size_type pos) const -> typename Items::const_reference {
    return items_[pos];
  }
  auto size() const -> typename Items::size_type { return items_.size(); }
  auto back() -> typename Items::reference {
    invalidate();
    return items_.back();
  }
  auto data() const noexcept -> const T * { return items_.data(); }
  void clear() {
    items_.clear();
    invalidate();
  }
  void push_back(const T &item) {
    items_.push_back(item);
    invalidate();
  }

  [[nodiscard]] auto modified() const -> TimePoint { return modified_; }

private:
  void invalidate() { modified_ = std::chrono::system_clock::now(); }
  Items items_;
  TimePoint modified_;
};
} // namespace mos
