/* 
 * File:   Text.cpp
 * Author: morgan
 * 
 * Created on August 28, 2013, 7:49 PM
 */

#include "graphics/text.hpp"

namespace mos {

    Text::Text(const std::string & txt,
               const CharMap & characters,
               const TexPtr & texture,
               const glm::mat4 & transform,
               const float spacing)
    : model_("Text",
             std::make_shared<Mesh>(Mesh()),
             texture,
             nullptr,
             transform),
      characters_(characters),
      spacing(spacing) {
        text(txt);
    }

    Text::~Text() {
    }
    
    std::string Text::text() const {
        return text_;
    }

    void Text::text(const std::string & text) {
                
        text_ = text;
        float index = 0.0f;
        model_.mesh->clear();
        for (const char & c : text_) {
            auto character = characters_.at(c);
            float u1 = character.rect_x / ((float) model_.texture->width());
            float u2 = (character.rect_x + character.rect_w) / (float) model_.texture->width();
            float v1 = character.rect_y / ((float) model_.texture->height());
            float v2 = ((character.rect_y + character.rect_h) / ((float) model_.texture->height()));

            float offset_y = ((float) model_.texture->height()) - character.offset_y;
            float offset_x = character.offset_x;
                        
            model_.mesh->add(Vertex(glm::vec3(index + offset_x, -character.rect_h - offset_y, 0.0f), glm::vec3(0.0f), glm::vec2(u1, v2)));
            model_.mesh->add(Vertex(glm::vec3(index + character.rect_w + offset_x, -offset_y, 0.0f), glm::vec3(0.0f), glm::vec2(u2, v1)));
            model_.mesh->add(Vertex(glm::vec3(index + offset_x, -offset_y, 0.0f), glm::vec3(0.0f), glm::vec2(u1, v1)));
            model_.mesh->add(Vertex(glm::vec3(index + offset_x, -character.rect_h - offset_y, 0.0f), glm::vec3(0.0f), glm::vec2(u1, v2)));
            model_.mesh->add(Vertex(glm::vec3(index + character.rect_w + offset_x, -character.rect_h - offset_y, 0.0f), glm::vec3(0.0f), glm::vec2(u2, v2)));
            model_.mesh->add(Vertex(glm::vec3(index + character.rect_w + offset_x, -offset_y, 0.0f), glm::vec3(0.0f), glm::vec2(u2, v1)));
            index += character.advance + spacing;
        }
        model_.mesh->valid = false;
        
    }

    float Text::width() {        
        glm::vec2 p1 = glm::vec2(model_.mesh->vertices_begin()->position);
        glm::vec2 p2 = glm::vec2((model_.mesh->vertices_end()-2)->position);
        return glm::distance(p1,p2);
    }

    Model Text::model() const {
        return model_;
    }
}
