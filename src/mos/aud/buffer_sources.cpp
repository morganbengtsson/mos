#include <mos/aud/buffer_sources.hpp>

namespace mos {
namespace aud {

BufferSources::BufferSources() {}

mos::aud::BufferSources::BufferSources(const std::initializer_list <BufferSource> &buffer_sources) :
    BufferSources(buffer_sources.begin(), buffer_sources.end()) {}

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

void BufferSources::push_back(const BufferSource &buffer_source) {
  buffer_sources_.push_back(buffer_source);
}

}
}