#pragma once

#include <initializer_list>
#include <vector>
#include <mos/aud/sound.hpp>
#include <mos/aud/listener.hpp>
#include <mos/aud/sound_stream.hpp>
#include <mos/aud/sounds.hpp>
#include <mos/aud/sound_streams.hpp>

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
