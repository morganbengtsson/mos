#include <mos/graphics/text.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace mos {

Text::Text(const std::string &txt, const Font &font, const glm::mat4 &transform,
           const float spacing)
    : model_("Text", std::make_shared<Mesh>(Mesh()), font.texture, nullptr,
             transform),
      font_(font), spacing(spacing) {
  model_.shader = Model::Shader::TEXT;
  model_.receives_light = false;
  text(txt);
}

Text::~Text() {}

std::string Text::text() const { return text_; }

void Text::text(const std::string &text) {

  text_ = text;
  float index = 0.0f;
  model_.mesh->clear();
  for (const char &c : text_) {
    auto character = font_.characters.at(c);
    float u1 = character.rect_x / ((float)model_.texture->width());
    float u2 =
        (character.rect_x + character.rect_w) / (float)model_.texture->width();
    float v1 = character.rect_y / ((float)model_.texture->height());
    float v2 = ((character.rect_y + character.rect_h) /
                ((float)model_.texture->height()));

    // float offset_y = ((float) model_.texture->height()) - character.offset_y;
    float offset_y = character.offset_y - font_.ascender();
    float offset_x = character.offset_x;
    float rect_h = -character.rect_h;

    model_.mesh->add(
        Vertex(glm::vec3(index + offset_x, rect_h + offset_y, 0.0f),
               glm::vec3(0.0f), glm::vec2(u1, v2)));
    model_.mesh->add(
        Vertex(glm::vec3(index + character.rect_w + offset_x, offset_y, 0.0f),
               glm::vec3(0.0f), glm::vec2(u2, v1)));
    model_.mesh->add(Vertex(glm::vec3(index + offset_x, offset_y, 0.0f),
                            glm::vec3(0.0f), glm::vec2(u1, v1)));
    model_.mesh->add(
        Vertex(glm::vec3(index + offset_x, rect_h + offset_y, 0.0f),
               glm::vec3(0.0f), glm::vec2(u1, v2)));
    model_.mesh->add(Vertex(
        glm::vec3(index + character.rect_w + offset_x, rect_h + offset_y, 0.0f),
        glm::vec3(0.0f), glm::vec2(u2, v2)));
    model_.mesh->add(
        Vertex(glm::vec3(index + character.rect_w + offset_x, offset_y, 0.0f),
               glm::vec3(0.0f), glm::vec2(u2, v1)));
    index += character.advance + spacing;
  }
  model_.mesh->invalidate();
}

float Text::width() const {
  glm::vec2 p1 = glm::vec2(model_.mesh->vertices_begin()->position);
  glm::vec2 p2 = glm::vec2((model_.mesh->vertices_end() - 2)->position);
  return glm::distance(p1, p2);
}

float Text::height() const { return font_.height(); }

void Text::position(const glm::vec2 &position) {
  model_.transform = glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, 0.0f));
}

glm::vec2 Text::position() { return glm::vec2(model_.position()); }

void Text::scale(const float scale) {
  model_.transform = glm::scale(model_.transform, glm::vec3(scale, scale, scale));
}

void Text::material(const std::shared_ptr<Material> &material) {
  model_.material = material;
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
