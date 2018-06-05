#pragma once

#include <initializer_list>
#include <mos/aud/buffer_source.hpp>

namespace mos {
namespace aud {

class BufferSources {
public:
  using Container = std::vector<BufferSource>;
  BufferSources();
  BufferSources(const std::initializer_list<BufferSource> &buffer_sources);
  template <class It>
  BufferSources(It begin, It end) : buffer_sources_(begin, end){}
  Container::const_iterator begin() const;
  Container::const_iterator end() const;
  Container::iterator begin();
  Container::iterator end();
  void push_back(const BufferSource& buffer_source);
private:
  Container buffer_sources_;
};
}
}