#pragma once
#include <mos/gfx/vertex.hpp>
#include <vector>
namespace mos {
namespace gfx {

/** Container with modified time stamp. */
template<class T>
class Container {
public:
  using Cont = std::vector<T>;

  Container(){};
  template<class It>

  Container(It begin, It end) : data_(begin, end){}
  template<class It>

  void assign(It begin, It end){
    data_.assign(begin, end);
  }
  typename Cont::iterator begin() {
    return data_.begin();
  }
  typename Cont::iterator end() {
    return data_.end();
  }
  typename Cont::const_iterator begin() const {
    return data_.begin();
  }
  typename Cont::const_iterator end() const {
    return data_.end();
  }
  typename Cont::reference operator[](typename Cont::size_type pos){
    return data_[pos];
  }
  typename Cont::size_type size() const{
    return data_.size();
  }
  typename Cont::reference back(){
    return data_.back();
  }
  const T * data() const noexcept{
    return data_.data();
  }
  void clear(){
    data_.clear();
  }
  void push_back(const Vertex &vertex){
    data_.push_back(vertex);
  }
private:
  std::vector<T> data_;
};

}
}