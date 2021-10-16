#pragma once

#include <glm/glm.hpp>
#include <memory>

#include <mos/aud/source.hpp>
#include <mos/aud/stream.hpp>

namespace mos {
namespace aud {

/** Stream audio from file, combined with source data. */
class Sound_stream final {
public:
  explicit Sound_stream(Shared_stream stream, Source source = Source());

  /** Stream used for the source. */
  Shared_stream stream;

  /** Source data. */
  Source source;
};
} // namespace aud
} // namespace mos
