#pragma once

#include <initializer_list>
#include <vector>
#include <mos/aud/stream_source.hpp>

namespace mos {
namespace aud {

class StreamSources {
public:
  using Container = std::vector<StreamSource>;
  StreamSources();
  StreamSources(const std::initializer_list<StreamSource> &stream_sources);
  template <class It>
  StreamSources(It begin, It end) : stream_sources_(begin, end){}
  Container::const_iterator begin() const;
  Container::const_iterator end() const;
  Container::iterator begin();
  Container::iterator end();
  void push_back(const StreamSource &stream_source);
private:
  Container stream_sources_;
};

}
}