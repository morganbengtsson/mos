#include <mos/gfx/text.hpp>
#include <mos/util.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>
#include <algorithm>

namespace mos {
namespace gfx {
Text::Text(const std::string &txt,
           const Font &font,
           const glm::mat4 &transform,
           const float spacing,
           const float opacity,
           const float roughness,
           const float metallic,
           const float emiss)
    : spacing(spacing),
      model_("Text", std::make_shared<Mesh>(Mesh()), transform),
      font_(font) {
  model_.material.roughness = roughness;
  model_.material.metallic = metallic;
  model_.material.albedo = glm::vec3(1.0f);
  model_.material.opacity = opacity;
  model_.material.emission = emiss;
  model_.material.albedo_map = font_.texture;
  text(txt);
}

std::string Text::text() const { return text_; }

void Text::text(const std::string &text) {
  if (text_.compare(text) != 0) {
    text_ = text;
    std::vector<std::string> lines = mos::split(text_, '\n');
    model_.mesh->clear();

    float line_index = 0.0f;
    const float line_height = -1.0f;
    int triangle_index = 0;
    for (auto &line : lines) {
      float index = 0.0f;
      for (auto &c : line) {
        auto character = font_.characters.at(c);
        float u1 = character.x / static_cast<float>(font_.texture->width());
        float u2 = (character.x + character.width) /
            static_cast<float>(font_.texture->width());
        float v1 = character.y / static_cast<float>(font_.texture->height());
        float v2 = ((character.y + character.height) /
            static_cast<float>(font_.texture->height()));

        float offset_y = -(character.y_offset - font_.base()) / font_.height();
        float offset_x = character.x_offset / font_.height();
        float rect_h = -character.height / font_.height();
        float rect_w = character.width / font_.height();
        float advance = character.x_advance / font_.height();

        float z = index / 2000.0f;

        const glm::vec3 normal = glm::vec3(0.0f, 0.0f, 1.0f);
        const glm::vec3 tangent = glm::vec3(0.0f, 1.0f, 0.0f);

        model_.mesh->vertices.push_back(
            Vertex(glm::vec3(index + offset_x, rect_h + offset_y + line_index, z),
                   normal, tangent, glm::vec2(u1, v2)));

        model_.mesh->vertices.push_back(Vertex(
            glm::vec3(index + rect_w + offset_x, offset_y + line_index, z),
            normal, tangent, glm::vec2(u2, v1)));

        model_.mesh->vertices.push_back(Vertex(glm::vec3(index + offset_x, offset_y + line_index, z),
                                               normal, tangent, glm::vec2(u1, v1)));

        model_.mesh->triangles.push_back({triangle_index++, triangle_index++, triangle_index++});

        model_.mesh->vertices.push_back(Vertex(glm::vec3(index + rect_w + offset_x,
                                                         rect_h + offset_y + line_index, z),
                                               normal, tangent, glm::vec2(u2, v2)));

        model_.mesh->triangles.push_back({triangle_index - 3, triangle_index++, triangle_index - 3});

        index += advance + spacing;
      }
      line_index += line_height;
    }
  }
}

float Text::width() const {
  if (model_.mesh->vertices.size() > 2) {
    auto result = std::minmax_element(model_.mesh->vertices.begin(),
                                      model_.mesh->vertices.end(),
                                      [](const auto &a, const auto &b) { return a.position.x < b.position.x; });

    return glm::distance(result.first->position.x, result.second->position.x);
  } else {
    return 0.0f;
  }
}

float Text::height() const {
  if (model_.mesh->vertices.size() > 2) {
    auto result = std::minmax_element(model_.mesh->vertices.begin(),
                                      model_.mesh->vertices.end(),
                                      [](const auto &a, const auto &b) { return a.position.y < b.position.y; });

    return glm::distance(result.first->position.t, result.second->position.y);
  } else {
    return 0.0f;
  }
}

void Text::position(const glm::vec2 &position) {
  model_.transform =
      glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, 0.0f));
}

void Text::position(const glm::vec3 &position) {
  model_.transform = glm::translate(glm::mat4(1.0f), position);
}

glm::vec2 Text::position() const { return glm::vec2(model_.position()); }

void Text::scale(const float scale) {
  model_.transform =
      glm::scale(model_.transform, glm::vec3(scale, scale, scale));
}

void Text::transform(const glm::mat4 &transform) {
  model_.transform = transform;
}

glm::mat4 Text::transform() const {
  return model_.transform;
}

Model Text::model() const { return model_; }

void Text::opacity(const float op) {
  model_.material.opacity = op;
}

Text &Text::operator=(const std::string &input) {
  text(input);
  return *this;
}

Text &Text::operator+=(const std::string &input) {
  text(text() + input);
  return *this;
}

void Text::emission(const float emiss) {
 model_.material.emission = emiss;
}

Font Text::font() const {
  return font_;
}
}
}
