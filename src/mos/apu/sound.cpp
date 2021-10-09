#include <mos/apu/sound.hpp>

mos::apu::Sound::Sound(const aud::Sound &sound) : buffer(*sound.buffer), source(sound.source) {

}
