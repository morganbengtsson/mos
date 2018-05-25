#include <mos/aud/buffer_sources.hpp>

namespace mos {
namespace aud {

mos::aud::BufferSources::BufferSources(const std::initializer_list <BufferSource> &buffer_sources) : buffer_sources_(buffer_sources) {

}

BufferSources::Container::const_iterator mos::aud::BufferSources::begin() const {
  return buffer_sources_.begin();
}

BufferSources::Container::const_iterator mos::aud::BufferSources::end() const {
  return buffer_sources_.end();
}

BufferSources::Container::iterator mos::aud::BufferSources::begin() {
  return buffer_sources_.begin();
}

BufferSources::Container::iterator mos::aud::BufferSources::end() {
  return buffer_sources_.end();
}

}
}