#include <mos/gfx/text.hpp>
#include <mos/util.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>
#include <algorithm>
#include <utf8.h>

namespace mos::gfx {
Text::Text(const std::string &txt,
           Font font,
           const glm::mat4 &transform,
           const float spacing,
           const float opacity,
           const float roughness,
           const float metallic,
           const glm::vec3 & emiss)
    : spacing(spacing),
      model_("Text", std::make_shared<Mesh>(Mesh()), transform),
      font_(std::move(font)) {
  model_.material.roughness.value = roughness;
  model_.material.metallic.value = metallic;
  model_.material.albedo.value = glm::vec3(1.0f);
  model_.material.index_of_refraction = 1.0f;
  model_.material.index_of_refraction = opacity;
  model_.material.emission.value = emiss;
  model_.material.albedo.texture = font_.texture;
  text(txt);
}

auto Text::text() const -> std::string { return text_; }

auto Text::text(const std::string &text) -> void {
  if (text_ != text) {
    text_ = text;

    model_.mesh->clear();

    float line_index = 0.0f;
    const float line_height = -1.0f;
    int triangle_index = 0;

    std::istringstream stream(text_);
    std::string line;
    while (std::getline(stream, line)) {
      float index = 0.0f;

      auto it = line.begin();
      while(it != line.end()) {
        auto cp = utf8::next(it, line.end());
        auto character = font_.characters.at(cp);
        const float u1 = character.position.x / static_cast<float>(font_.texture->width());
        const float u2 = (character.position.x + character.width) /
            static_cast<float>(font_.texture->width());
        const float v1 = character.position.y / static_cast<float>(font_.texture->height());
        const float v2 = ((character.position.y + character.height) /
            static_cast<float>(font_.texture->height()));

        const float offset_y = -(character.offset.y - font_.base()) / font_.height();
        const float offset_x = character.offset.x / font_.height();
        const float rect_h = -character.height / font_.height();
        const float rect_w = character.width / font_.height();
        const float advance = character.advance / font_.height();

        const float z = index / 2000.0f;

        const glm::vec3 normal = glm::vec3(0.0f, 0.0f, 1.0f);
        const glm::vec3 tangent = glm::vec3(0.0f, 1.0f, 0.0f);

        model_.mesh->vertices.push_back(
            Vertex{glm::vec3(index + offset_x, rect_h + offset_y + line_index, z),
                   normal, tangent, glm::vec2(u1, v2)});

        model_.mesh->vertices.push_back(Vertex{
            glm::vec3(index + rect_w + offset_x, offset_y + line_index, z),
            normal, tangent, glm::vec2(u2, v1)});

        model_.mesh->vertices.push_back(Vertex{glm::vec3(index + offset_x, offset_y + line_index, z),
                                               normal, tangent, glm::vec2(u1, v1)});

        model_.mesh->triangles.push_back({triangle_index++, triangle_index++, triangle_index++});

        model_.mesh->vertices.push_back(Vertex{glm::vec3(index + rect_w + offset_x,
                                                         rect_h + offset_y + line_index, z),
                                               normal, tangent, glm::vec2(u2, v2)});

        model_.mesh->triangles.push_back({triangle_index - 3, triangle_index++, triangle_index - 3});

        index += advance + spacing;
      }
      line_index += line_height;
    }
  }
}

auto Text::width() const -> float {
  if (model_.mesh->vertices.size() > 2) {
    auto result = std::minmax_element(model_.mesh->vertices.begin(),
                                      model_.mesh->vertices.end(),
                                      [](const auto &a, const auto &b) { return a.position.x < b.position.x; });

    return glm::distance(result.first->position.x, result.second->position.x);
  }
  return 0.0f;
}

auto Text::height() const -> float {
  if (model_.mesh->vertices.size() > 2) {
    auto result = std::minmax_element(model_.mesh->vertices.begin(),
                                      model_.mesh->vertices.end(),
                                      [](const auto &a, const auto &b) { return a.position.y < b.position.y; });

    return glm::distance(result.first->position.t, result.second->position.y);
  }
  return 0.0f;
}

auto Text::position(const glm::vec2 &position) -> void {
  model_.transform =
      glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, 0.0f));
}

auto Text::position(const glm::vec3 &position) -> void {
  model_.transform = glm::translate(glm::mat4(1.0f), position);
}

auto Text::position() const -> glm::vec2 { return glm::vec2(model_.position()); }

auto Text::scale(const float scale) -> void {
  model_.transform =
      glm::scale(model_.transform, glm::vec3(scale, scale, scale));
}

auto Text::transform(const glm::mat4 &transform) -> void {
  model_.transform = transform;
}

auto Text::transform() const -> glm::mat4 {
  return model_.transform;
}

auto Text::model() const -> Model { return model_; }

auto Text::opacity(const float op) -> void {
  model_.material.transmission = 1.0f - op;
}

auto Text::operator=(const std::string &input) -> Text & {
  text(input);
  return *this;
}

auto Text::operator+=(const std::string &input) -> Text & {
  text(text() + input);
  return *this;
}

auto Text::emission(const glm::vec3 & emiss) -> void {
 model_.material.emission.value = emiss;
}

auto Text::font() const -> Font {
  return font_;
}
}
