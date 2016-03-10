/*
 * File:   Texture.cpp
 * Author: morgan
 *
 * Created on February 25, 2014, 7:26 PM
 */

#include <mos/graphics/texture2d.hpp>
#include <lodepng.h>
#include <iostream>

namespace mos {

  unsigned int Texture2D::current_id = 0;
  Texture2D::Texture2D(const std::string & path, const bool mips) {
    auto error =
        lodepng::decode(texels_, width_, height_, path);
    if (error) {
      std::cout << "Decoder error: " << error << ": " << lodepng_error_text(error)
                << std::endl;
    }
    mipmaps = mips;
    id_ = current_id++;
  }

  Texture2D::~Texture2D() {}

Texture2D::Texels::const_iterator Texture2D::begin() const {
  return texels_.begin();
}

Texture2D::Texels::const_iterator Texture2D::end() const {
  return texels_.end();
}

unsigned int Texture2D::id() const { return id_; }

unsigned int Texture2D::width() const { return width_; }

unsigned int Texture2D::height() const { return height_; }

unsigned int Texture2D::size() const { return texels_.size(); }

glm::vec4 Texture2D::sample(const unsigned int x, const unsigned int y) {
  unsigned int index = x * 4 * height() + y * 4;
  unsigned char r = texels_[index];
  unsigned char g = texels_[index + 1];
  unsigned char b = texels_[index + 2];
  unsigned char a = texels_[index + 3];
  return glm::vec4((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f,
                   (float)a / 255.0f);
}

const unsigned char *Texture2D::data() const { return texels_.data(); }
}
