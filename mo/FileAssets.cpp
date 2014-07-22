/* 
 * File:   Assets.cpp
 * Author: morgan
 * 
 * Created on February 25, 2014, 6:38 PM
 */

#include "FileAssets.h"

#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>
#include <objload.h>
#include <lodepng.h>
#include <exception>
#include <system_error>
#include <memory>
#include <fstream>
#include <iostream>
#include <iterator>

using namespace mo;
using namespace std;
using namespace glm;

namespace mo {

    FileAssets::FileAssets() :
    directory_("assets/") {
        vector<Vertex> vertices;
        vector<int> elements;
        models_.insert(MeshPair("Empty.obj", std::make_shared<Mesh>(Mesh(vertices.begin(),
                vertices.end(),
                elements.begin(),
                elements.end()))));
    }

    FileAssets::~FileAssets() {
    }

    std::shared_ptr<Mesh> FileAssets::mesh(std::string file_name) {
        if (models_.find(file_name) == models_.end()) {
            vector<mo::Vertex> vertices;
            vector<int> elements;

            cout << "Loading: " << directory_ << file_name << std::endl;

            obj::Model obj_model = obj::loadModelFromFile(directory_ + file_name);
            int j = 0;
            for (int i = 0; i < obj_model.vertex.size(); i += 3) {
                glm::vec3 position(obj_model.vertex[i], obj_model.vertex[i + 1], obj_model.vertex[i + 2]);
                glm::vec3 normal(obj_model.normal[i], obj_model.normal[i + 1], obj_model.normal[i + 2]);
                glm::vec2 uv(0.0f, 0.0f);
                if (obj_model.texCoord.size() > 0){
                    uv.x = obj_model.texCoord[j];
                    uv.y = -obj_model.texCoord[j + 1];
                }
                
                j += 2;
                vertices.push_back(Vertex(position, normal, uv));
            }
            elements.assign(obj_model.faces.find("default")->second.begin(),
                    obj_model.faces.find("default")->second.end());

            models_.insert(MeshPair(file_name, std::make_shared<Mesh>(mo::Mesh(vertices.begin(),
                    vertices.end(),
                    elements.begin(),
                    elements.end()))));
            return models_.at(file_name);
        } else {
            return models_.at(file_name);
        }
    }

    std::shared_ptr<Texture2D> FileAssets::texture(std::string file_name) {
        using namespace mo;
        if (textures_.find(file_name) == textures_.end()) {
            std::vector<unsigned char> texels;
            unsigned width, height;
            std::cout << "Loading: " << directory_ + file_name << std::endl;
            unsigned error = lodepng::decode(texels, width, height, directory_ + file_name);
            if (error) {
                std::cout << "Decoder error: " << error << ": " << lodepng_error_text(error) << std::endl;

            }
            textures_.insert(TexturePair(file_name, std::make_shared<Texture2D>(Texture2D(texels.begin(), texels.end(), width, height))));
            return textures_.at(file_name);
        } else {
            return textures_.at(file_name);
        }
    }

    std::shared_ptr<Sound> FileAssets::sound(std::string file_name) {
        if (sounds_.find(file_name) == sounds_.end()) {
            std::ifstream file(directory_ + file_name, std::ios::binary);

            std::vector<int> data;

            int v;
            while (file.read(reinterpret_cast<char*> (&v), sizeof (v))) {
                data.push_back(v);
            }

            sounds_.insert(SoundPair(file_name, std::make_shared<Sound>(Sound(data.begin(), data.end()))));
            return sounds_.at(file_name);
        } else {
            return sounds_.at(file_name);
        }
    }

    std::string FileAssets::text(std::string file_name) {
        std::ifstream file(directory_ + file_name);
        std::string source((std::istreambuf_iterator<char>(file)),
                std::istreambuf_iterator<char>());
        return source;
    }

    std::map<char, Character> FileAssets::characterMap(std::string file_name) {
        
        std::map<char, Character> characters;
        rapidxml::xml_document<> doc;
        //doc.parse<0>((char*) text(path));
        std::string str = text(file_name);
        char* cstr = new char[str.size() + 1];  // Create char buffer to store string copy
        strcpy (cstr, str.c_str()); 
        doc.parse<0>(cstr);
        
        delete [] cstr;

        std::cout << "Name of my first node is: " << doc.first_node()->name() << "\n";

        //width_ = atoi(doc.first_node("font")->first_node("texture")->first_attribute("width")->value());
        //height_ = atoi(doc.first_node("font")->first_node("texture")->first_attribute("height")->value());

        rapidxml::xml_node<> * chars_node = doc.first_node("font")->first_node("chars");

        for (rapidxml::xml_node<> * char_node = chars_node->first_node("char");
                char_node;
                char_node = char_node->next_sibling()) {

            Character character;
            rapidxml::xml_attribute<> *attr = char_node->first_attribute();
            character.offsetX = atof(attr->value());
            attr = attr->next_attribute();
            character.offsetY = atof(attr->value());
            attr = attr->next_attribute();
            character.advance = atof(attr->value());
            attr = attr->next_attribute();
            character.rectW = atof(attr->value());
            attr = attr->next_attribute();
            character.id = *attr->value();
            attr = attr->next_attribute();
            character.rectX = atof(attr->value());
            attr = attr->next_attribute();
            character.rectY = atof(attr->value());
            attr = attr->next_attribute();
            character.rectH = atof(attr->value());

            characters.insert(std::pair<char, Character>(character.id, character));

        }
        return characters;
    }
}
