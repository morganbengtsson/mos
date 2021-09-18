#pragma once

#include <glad/glad.h>

struct Light_uniforms {
  GLint position;
  GLint color;
  GLint strength;
  GLint view;
  GLint projection;
  GLint angle;
  GLint direction;
};
