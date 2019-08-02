#pragma once

#include <glm/glm.hpp>
#include <memory>

#include <mos/aud/stream.hpp>
#include <mos/aud/source.hpp>

namespace mos {
namespace aud {

/** Stream audio from file, combined with source data. */
class Stream_speaker final {
public:
  explicit Stream_speaker(Shared_stream stream,
                         Source source = Source());

  /** Stream used for the source. */
  Shared_stream stream;

  /** Source data. */
  Source source;
};
}
}
