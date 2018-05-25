#pragma once

#include <initializer_list>
#include <mos/aud/buffer_source.hpp>

namespace mos {
namespace aud {

class BufferSources {
public:
  using Container = std::vector<BufferSource>;
  BufferSources(const std::initializer_list<BufferSource> &buffer_sources);
  Container::const_iterator begin() const;
  Container::const_iterator end() const;
  Container::iterator begin();
  Container::iterator end();
private:
  Container buffer_sources_;
};

}
}