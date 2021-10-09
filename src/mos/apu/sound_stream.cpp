#include <mos/apu/sound_stream.hpp>

mos::apu::Sound_stream::Sound_stream(const aud::Sound_stream& sound_stream)
    : stream(sound_stream.stream), source(sound_stream.source) {}
