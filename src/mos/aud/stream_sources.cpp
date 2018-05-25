#include <mos/aud/stream_sources.hpp>

namespace mos {
namespace aud {

StreamSources::StreamSources(const std::initializer_list <StreamSource> &stream_sources) :
    stream_sources_(stream_sources) {}

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

}
}