#pragma once
#include <mos/gfx/vertex.hpp>
#include <vector>
namespace mos {
namespace gfx {

/** Container for vertices. */
class Vertices {
public:
  using Container = std::vector<Vertex>;
  Vertices();
  template<class It>
  Vertices(It begin, It end) : vertices_(begin, end){}
  template<class It>
  void assign(It begin, It end){
    vertices_.assign(begin, end);
  }
  Container::iterator begin();
  Container::iterator end();
  Container::const_iterator begin() const;
  Container::const_iterator end() const;
  Container::reference operator[](Container::size_type pos);
  const Vertex * data() const noexcept;
  Container::size_type size() const;
  void clear();
  void push_back(const Vertex &vertex);
  Container::reference back();

private:
  std::vector<Vertex> vertices_;
};

}
}