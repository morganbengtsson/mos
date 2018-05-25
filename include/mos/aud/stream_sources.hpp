#pragma once

#include <initializer_list>
#include <vector>
#include <mos/aud/stream_source.hpp>

namespace mos {
namespace aud {

class StreamSources {
public:
  using Container = std::vector<StreamSource>;
  StreamSources(const std::initializer_list<StreamSource> &stream_sources);
  Container::const_iterator begin() const;
  Container::const_iterator end() const;
  Container::iterator begin();
  Container::iterator end();
private:
  Container stream_sources_;
};

}
}