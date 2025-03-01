#pragma once

#include <glm/glm.hpp>

#include <mos/aud/source.hpp>
#include <mos/aud/stream.hpp>

namespace mos::aud {

/** Stream audio from file, combined with source data. */
class Sound_stream final {
public:
  explicit Sound_stream(Shared_stream stream, Source source = Source());

  /** Stream used for the source. */
  Shared_stream stream;

  /** Source data. */
  Source source;
};
} // namespace mos::aud

