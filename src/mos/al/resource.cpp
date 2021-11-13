#include <mos/al/resource.hpp>

namespace mos::al {

Resource::Resource(const Gen_function& gen_function, const Delete_function& delete_function)
    : gen_function(gen_function), delete_function(delete_function) {
  gen_function(1, &id);
}

Resource::Resource(Resource &&resource) noexcept : id(resource.id) {
  resource.id = 0;
}

Resource::~Resource() { release(); }

auto Resource::operator=(Resource &&resource) noexcept -> Resource & {
  if (this != &resource) {
    release();
    std::swap(id, resource.id);
  }
  return *this;
}

void Resource::release() {
  if (delete_function) {
    delete_function(1, &id);
  }
  id = 0;
}
} // namespace mos::al
