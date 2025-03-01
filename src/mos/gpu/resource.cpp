#include <mos/gpu/resource.hpp>

namespace mos::gpu {

Resource::Resource(const int id) : id_(id) {

}

auto Resource::valid() const -> bool {
  return id_ != invalid_id;
}

auto Resource::id() const -> decltype(id_){
  return id_;
}
} // namespace mos::gpu
