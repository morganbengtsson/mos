#include <mos/gfx/text.hpp>
#include <mos/util.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
namespace mos {
namespace gfx {
Text::Text(const std::string &txt, const Font &font, const glm::mat4 &transform,
           const float spacing)
    : model_("Text", std::make_shared<Mesh>(Mesh()),
             transform),
      font_(font), spacing(spacing) {
  model_.material.albedo = glm::vec3(1.0f);
  model_.material.opacity = 0.0f;
  model_.material.emission = glm::vec3(1.0f);
  model_.material.albedo_map = font.texture;
  text(txt);
}

Text::~Text() {}

std::string Text::text() const { return text_; }

void Text::text(const std::string &text) {
  if (text_.compare(text) != 0) {
    text_ = text;
    std::vector<std::string> lines = mos::split(text_, '\n');
    model_.mesh->clear();

    float line_index = 0.0f;
    const float line_height = -font_.height();
    for (auto & line : lines) {
      float index = 0.0f;
      for (auto & c : line) {
        auto character = font_.characters.at(c);
        float u1 = character.rect_x / ((float)model_.material.albedo_map->width());
        float u2 = (character.rect_x + character.rect_w) /
                   (float)model_.material.albedo_map->width();
        float v1 = character.rect_y / ((float)model_.material.albedo_map->height());
        float v2 = ((character.rect_y + character.rect_h) /
                    ((float)model_.material.albedo_map->height()));

        float offset_y = character.offset_y;
        float offset_x = character.offset_x;
        float rect_h = -character.rect_h;
        float rect_w = character.rect_w;

        float z = index / 2000.0f;
        model_.mesh->vertices.push_back(
            Vertex(glm::vec3(index + offset_x, rect_h + offset_y + line_index, z),
                   glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f), glm::vec2(u1, v2)));

        model_.mesh->vertices.push_back(Vertex(
            glm::vec3(index + rect_w + offset_x, offset_y + line_index, z),
            glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f), glm::vec2(u2, v1)));

        model_.mesh->vertices.push_back(Vertex(glm::vec3(index + offset_x, offset_y + line_index, z),
                                glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f), glm::vec2(u1, v1)));
        model_.mesh->vertices.push_back(
            Vertex(glm::vec3(index + offset_x, rect_h + offset_y + line_index, z),
                   glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f), glm::vec2(u1, v2)));

        model_.mesh->vertices.push_back(Vertex(glm::vec3(index + rect_w + offset_x,
                                          rect_h + offset_y + line_index, z),
                                glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f), glm::vec2(u2, v2)));
        model_.mesh->vertices.push_back(Vertex(
            glm::vec3(index + rect_w + offset_x, offset_y + line_index, z),
            glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f), glm::vec2(u2, v1)));

        index += character.advance + spacing;
      }
      line_index += line_height;
    }
    model_.mesh->invalidate();
  }
}

float Text::width() const {
  if (model_.mesh->vertices.size() > 2){
    glm::vec2 p1 = glm::vec2(model_.mesh->vertices.begin()->position);
    glm::vec2 p2 = glm::vec2((model_.mesh->vertices.end() - 2)->position);
    return glm::distance(p1, p2);
  }
  else {
    return 0.0f;
  }
}

float Text::height() const { return font_.height(); }

void Text::position(const glm::vec2 &position) {
  model_.transform =
      glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, 0.0f));
}

void Text::position(const glm::vec3 & position) {
  model_.transform = glm::translate(glm::mat4(1.0f), position);
}

glm::vec2 Text::position() const { return glm::vec2(model_.position()); }

void Text::scale(const float scale) {
  model_.transform =
      glm::scale(model_.transform, glm::vec3(scale, scale, scale));
}

void Text::material(const Material &material) {
  model_.material = material;
}

void Text::transform(const glm::mat4 & transform) {
  model_.transform = transform;
}

glm::mat4 Text::transform() const {
  return model_.transform;
}

Model Text::model() const { return model_; }


Text &Text::operator=(const std::string &input) {
  text(input);
  return *this;
}

Text &Text::operator+=(const std::string &input) {
  text(text() + input);
  return *this;
}
}
}
