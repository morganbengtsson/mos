/* 
 * File:   AndroidAssets.cpp
 * Author: morgan
 * 
 * Created on June 12, 2014, 2:29 PM
 */

#ifdef __ANDROID__

#include "AndroidAssets.h"
#include "Vertex.h"
#include "objload.h"
#include <glm/glm.hpp>
#include <vector>
#include <lodepng.h>
#include <rapidxml.hpp>
#include "logging.h"

using namespace std;
using namespace glm;

namespace mo {

    AndroidAssets::AndroidAssets(AAssetManager * manager) : manager_(manager) {
    }

    AndroidAssets::~AndroidAssets() {
    }

    std::shared_ptr<Mesh> AndroidAssets::mesh(std::string file_name) {
 
        string source = text(file_name);
        
        if (models_.find(file_name) == models_.end()) {
            vector<mo::Vertex> vertices;
            vector<int> elements;

            cout << "Loading: " << file_name << std::endl;

            obj::Model obj_model = obj::loadModelFromString(source);
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
        }
        return models_.at(file_name);
    }

    std::shared_ptr<Texture2D> AndroidAssets::texture(std::string file_name) {
        LOGI("Loading: %s", file_name.c_str());
        if (textures_.find(file_name) == textures_.end()) {
            std::cout << "Loading: " << file_name << std::endl;
            AAsset* asset = AAssetManager_open(manager_, file_name.c_str(), AASSET_MODE_UNKNOWN);
            int size = (int)AAsset_getLength(asset);
            
            const unsigned char * texels_encoded = static_cast<const unsigned char *>(AAsset_getBuffer(asset));
            vector<unsigned char> texels_decoded;
            unsigned width, height;            
            unsigned error = lodepng::decode(texels_decoded, width, height, texels_encoded, size);
            if (error) {
                std::cout << "Decoder error: " << error << ": " << lodepng_error_text(error) << std::endl;
                
            }
            textures_.insert(TexturePair(file_name, std::make_shared<Texture2D>(Texture2D(texels_decoded.begin(), texels_decoded.end(), width, height))));
            //LOGI("Size: %d", std::distance(texels_decoded.begin(), texels_decoded.end()));
        }       
        return textures_.at(file_name);
    }

    std::shared_ptr<Sound> AndroidAssets::sound(std::string file_name) {
        
        if (sounds_.find(file_name) == sounds_.end()) {
           
            AAsset* asset = AAssetManager_open(manager_, file_name.c_str(), AASSET_MODE_UNKNOWN);
            long size = AAsset_getLength(asset);
            const char * buffer = static_cast<const char *>(AAsset_getBuffer(asset));
            
            sounds_.insert(SoundPair(file_name, std::make_shared<Sound>(Sound(buffer, buffer+size))));
            return sounds_.at(file_name);
        } else {
            return sounds_.at(file_name);
        }        
    }
    
    std::string AndroidAssets::text(std::string file_name){
        AAsset* text = AAssetManager_open(manager_, file_name.c_str(), AASSET_MODE_UNKNOWN);
        long size = AAsset_getLength(text);
        char * buffer = new char[size + 1];
        buffer[size] = 0;
        AAsset_read(text, buffer, size);
        
        return std::string(buffer, buffer + size);
    }
    std::map<char, Character> AndroidAssets::characterMap(std::string file_name) {
        
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
#endif



