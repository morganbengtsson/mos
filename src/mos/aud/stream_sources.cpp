#include <mos/aud/stream_sources.hpp>

namespace mos {
namespace aud {

StreamSources::StreamSources() {}

StreamSources::StreamSources(const std::initializer_list <StreamSource> &stream_sources) :
   StreamSources(stream_sources.begin(), stream_sources.end()) {}

StreamSources::Container::const_iterator mos::aud::StreamSources::begin() const {
  return stream_sources_.begin();
}

StreamSources::Container::const_iterator mos::aud::StreamSources::end() const {
  return stream_sources_.end();
}

StreamSources::Container::iterator mos::aud::StreamSources::begin() {
  return stream_sources_.begin();
}

StreamSources::Container::iterator mos::aud::StreamSources::end() {
  return stream_sources_.end();
}

void StreamSources::push_back(const StreamSource &stream_source) {
  stream_sources_.push_back(stream_source);
}

}
}