#pragma once

#include <initializer_list>
#include <vector>

namespace mos{
namespace gfx{

class Model;

class Models {
public:
  using Items = std::vector<Model>;
  template<class It>
  Models(It begin, It end) : models_(begin, end){};
  Models(const std::initializer_list<Model> &models);
  Models();
  void push_back(const Model& model);
  Items::const_iterator begin() const;
  Items::const_iterator end() const;
  Items::iterator begin();
  Items::iterator end();
  Items::size_type size() const;
  Model& operator[](unsigned long pos);
private:
  Items models_;
};

}
}