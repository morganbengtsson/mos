#pragma once

#include <mos/apu/source.hpp>
#include <mos/aud/stream.hpp>
#include <mos/aud/sound_stream.hpp>

namespace mos::apu {

/** Streamed audio from file, combined with a source living on the APU */
class Sound_stream final {
  friend class mos::al::Renderer;
public:
  /** Stream data */
  aud::Shared_stream stream;

  /** Source on APU */
  Source source;
private:
  explicit Sound_stream(const aud::Sound_stream& sound_stream);
};
}
