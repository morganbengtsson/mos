#include <mos/gfx/models.hpp>
#include <mos/gfx/model.hpp>

namespace mos{
namespace gfx{

Models::Models(const std::initializer_list<Model> &models): models_(models.begin(), models.end()){
}

void Models::push_back(const Model &model) {
  models_.push_back(model);
}

Models::Models() {}

Models::Container::const_iterator Models::begin() const {
  return models_.begin();
}

Models::Container::const_iterator Models::end() const {
  return models_.end();
}
Model &Models::operator[](unsigned long pos) {
  return models_[pos];
}

}
}