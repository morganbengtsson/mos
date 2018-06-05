#include <mos/gfx/texture_targets.hpp>

namespace mos {
namespace gfx {

TextureTargets::TextureTargets(const std::initializer_list<TextureTargets> & texture_targets) :
    texture_targets_(texture_targets_) {}

TextureTargets::TextureTargets() {}

void TextureTargets::push_back(const TextureTarget &texture_target){
  texture_targets_.push_back(texture_target);
}
TextureTargets::Container::const_iterator TextureTargets::begin() const {
  return texture_targets_.begin();
}

TextureTargets::Container::const_iterator TextureTargets::end() const {
  return texture_targets_.end();
}
TextureTargets::Container::iterator TextureTargets::begin() {
  return texture_targets_.end();
}

TextureTargets::Container::iterator TextureTargets::end() {
  return texture_targets_.end();
}

TextureTarget &TextureTargets::operator[](unsigned long pos) {
  return texture_targets_[pos];
}
};
}
