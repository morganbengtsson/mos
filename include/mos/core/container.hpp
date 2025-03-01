#pragma once
#include <initializer_list>
#include <mos/core/tracked_container.hpp>
#include <vector>

namespace mos {

template <class T> class Tracked_container;

/** Container. */
template <class T> class Container {
public:
  using Items = std::vector<T>;
  using size_type = typename Items::size_type;

  Container() = default;

  template <class It>
  explicit Container(const std::initializer_list<It> list)
      : Container(list.begin(), list.end()) {}

  template <class It> Container(It begin, It end) { assign(begin, end); }

  explicit Container(const Tracked_container<T> &container)
      : Container(container.begin(), container.end()) {}

  template <class It> void assign(It begin, It end) {
    items_.assign(begin, end);
  }

  template <class It>
  auto insert(typename Items::const_iterator pos, It begin, It end) ->
      typename Items::iterator {
    return items_.insert(pos, begin, end);
  }

  auto begin() -> typename Items::iterator { return items_.begin(); }
  auto end() -> typename Items::iterator { return items_.end(); }
  auto begin() const -> typename Items::const_iterator {
    return items_.begin();
  }
  auto end() const -> typename Items::const_iterator { return items_.end(); }
  void assign(const std::initializer_list<T> &list) { items_.assign(list); }
  auto operator[](typename Items::size_type pos) -> typename Items::reference {
    return items_[pos];
  }
  auto operator[](typename Items::size_type pos) const ->
      typename Items::const_reference {
    return items_[pos];
  }
  auto size() const -> typename Items::size_type { return items_.size(); }
  auto back() -> typename Items::reference { return items_.back(); }
  auto data() const noexcept -> const T * { return items_.data(); }
  void clear() { items_.clear(); }

  void push_back(const T &item) { items_.push_back(item); }

  void append(const Container<T> &container) {
    insert(end(), container.begin(), container.end());
  }

private:
  Items items_;
};

} // namespace mos
