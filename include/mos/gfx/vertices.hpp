#pragma once
#include <mos/gfx/vertex.hpp>
#include <vector>
namespace mos {
namespace gfx {

/** Container for vertices. */
class Vertices {
public:
  using Container = std::vector<Vertex>;
  template<class T>
  Vertices(T begin, T end) : vertices_(begin, end){}
  Container::iterator begin();
  Container::iterator end();
  Container::const_iterator begin() const;
  Container::const_iterator end() const;
private:
  std::vector<Vertex> vertices_;
};

}
}