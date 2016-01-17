/*
 * File:   Sound.cpp
 * Author: morgan
 *
 * Created on May 6, 2014, 9:38 PM
 */

#include <mos/audio/sound.hpp>

namespace mos {
Sound::~Sound() {}

unsigned int Sound::id() const { return id_; }

bool Sound::valid() const { return valid_; }

unsigned int Sound::channels() const { return channels_; }

unsigned int Sound::sample_rate() const { return sample_rate_; }

const short *Sound::data() const { return samples_.data(); }
}
