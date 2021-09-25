#include <mos/al/filter.hpp>

namespace mos::al {

Filter::Filter(Filter &&filter) noexcept : id(filter.id) {
  filter.id = 0;
}

Filter &Filter::operator=(Filter &&filter) noexcept {
  if (this != &filter) {
    release();
    std::swap(id, filter.id);
  }
  return *this;
}

Filter::~Filter() {
  release();
}

Filter::Filter() {
  alGenFilters(1, &id);
}

void Filter::release() {
  alDeleteFilters(1, &id);
  id = 0;
}
}
