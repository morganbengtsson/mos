#pragma once

#include <initializer_list>
#include <vector>
#include <mos/aud/speaker.hpp>
#include <mos/aud/listener.hpp>
#include <mos/aud/stream_speaker.hpp>
#include <mos/aud/speakers.hpp>
#include <mos/aud/stream_speakers.hpp>

namespace mos {
namespace aud {

/** Audio scene with listener and audio sources. */
class Scene final {
public:
  Scene(Speakers speakers,
        Stream_speakers stream_speakers,
        Listener listener);
  Scene();

  Speakers speakers;
  Stream_speakers stream_speakers;
  Listener listener;
};
}
}
