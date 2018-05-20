#pragma once

#include <initializer_list>
#include <vector>

namespace mos{
namespace gfx{

class Model;

class Models {
public:
  using Container = std::vector<Model>;
  template<class It>
  Models(It begin, It end) : models_(begin, end){};
  Models(const std::initializer_list<Model> &models);
  Models();
  void push_back(const Model& model);
  Container::const_iterator begin() const;
  Container::const_iterator end() const;
  Container::iterator begin();
  Container::iterator end();
  Model& operator[](unsigned long pos);
private:
  std::vector<Model> models_;
};

}
}