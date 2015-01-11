/* 
 * File:   Text.cpp
 * Author: morgan
 * 
 * Created on August 28, 2013, 7:49 PM
 */

#include "text.hpp"

#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>
#include <fstream>
#include <streambuf>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/io.hpp>
#include <iostream>

namespace mo {

    Text::Text(const std::string txt, std::map<char, Character> characters, std::shared_ptr<Texture2D> texture, const glm::mat4 transform, const float spacing)
    : Model(std::shared_ptr<Mesh>(new Mesh()), texture, transform), characters_(characters), spacing(spacing) {
        text(txt);
    }

    Text::~Text() {
    }
    
    std::string Text::text() const {
        return text_;
    }

    void Text::text(const std::string text) {
                
        text_ = text;
        float index = 0.0f;
        mesh->clear();
        for (const char & c : text_) {
            auto character = characters_.at(c);
            float u1 = character.rectX / ((float) texture->width());
            float u2 = (character.rectX + character.rectW) / (float) texture->width();
            float v1 = character.rectY / ((float) texture->height());
            float v2 = ((character.rectY + character.rectH) / ((float) texture->height()));

            float offset_y = ((float) texture->height()) - character.offsetY;
            float offset_x = character.offsetX;
                        
            mesh->add(Vertex(glm::vec3(index + offset_x, -character.rectH - offset_y, 0.0f), glm::vec3(0.0f), glm::vec2(u1, v2)));
            mesh->add(Vertex(glm::vec3(index + character.rectW + offset_x, -offset_y, 0.0f), glm::vec3(0.0f), glm::vec2(u2, v1)));
            mesh->add(Vertex(glm::vec3(index + offset_x, -offset_y, 0.0f), glm::vec3(0.0f), glm::vec2(u1, v1)));
            mesh->add(Vertex(glm::vec3(index + offset_x, -character.rectH - offset_y, 0.0f), glm::vec3(0.0f), glm::vec2(u1, v2)));
            mesh->add(Vertex(glm::vec3(index + character.rectW + offset_x, -character.rectH - offset_y, 0.0f), glm::vec3(0.0f), glm::vec2(u2, v2)));
            mesh->add(Vertex(glm::vec3(index + character.rectW + offset_x, -offset_y, 0.0f), glm::vec3(0.0f), glm::vec2(u2, v1)));
            index += character.advance + spacing;
        }
        mesh->valid = false;
        
    }

    float Text::width() {        
        glm::vec2 p1 = glm::vec2(mesh->vertices_begin()->position);
        glm::vec2 p2 = glm::vec2((mesh->vertices_end()-2)->position);
        return glm::distance(p1,p2);
    }


}
