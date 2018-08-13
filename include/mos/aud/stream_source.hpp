#pragma once

#include <glm/glm.hpp>
#include <memory>

#include <mos/aud/stream.hpp>
#include <mos/aud/source.hpp>

namespace mos {
namespace aud {

/** Stream audio from file, combined with source data. */
class StreamSource final {
public:
  explicit StreamSource(const SharedStream &stream,
                        const Source &source = Source());
  ~StreamSource();

  /** Stream used for the source. */
  SharedStream stream;

  /** Source data. */
  Source source;
};
}
}
