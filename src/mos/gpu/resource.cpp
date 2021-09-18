#include <mos/gpu/resource.hpp>

namespace mos::gpu {

Resource::Resource(const int id) : id_(id) {

}

bool Resource::valid() const {
  return id_ != invalid_id;
}

int Resource::id() const {
  return id_;
}
}
