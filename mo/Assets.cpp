/* 
 * File:   Assets.cpp
 * Author: morgan
 * 
 * Created on February 25, 2014, 6:38 PM
 */

#include "Assets.h"
#include "logging.h"

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
#include <stb_vorbis.c>

using namespace mo;
using namespace std;
using namespace glm;

namespace mo {

#ifdef __ANDROID__

    Assets::Assets(AAssetManager * manager) : manager_(manager) {
    }
#else

    Assets::Assets() :
    directory_("assets/") {
        vector<Vertex> vertices;
        vector<int> elements;
        models_.insert(MeshPair("Empty.obj", std::make_shared<Mesh>(Mesh(vertices.begin(),
                vertices.end(),
                elements.begin(),
                elements.end()))));
    }
#endif

    Assets::~Assets() {
    }

    std::shared_ptr<Mesh> Assets::mesh(std::string file_name) {

        if (models_.find(file_name) == models_.end()) {
            vector<mo::Vertex> vertices;
            vector<int> elements;

#ifdef __ANDROID__
            LOGI("Loading: %s", file_name.c_str());
            std::string source = text(file_name);
            obj::Model obj_model = obj::loadModelFromString(source);
#else
            LOGI("Loading: %s%s", directory_.c_str(),file_name.c_str());
            obj::Model obj_model = obj::loadModelFromFile(directory_ + file_name);
#endif
            int j = 0;
            for (int i = 0; i < obj_model.vertex.size(); i += 3) {
                glm::vec3 position(obj_model.vertex[i], obj_model.vertex[i + 1], obj_model.vertex[i + 2]);
                glm::vec3 normal(obj_model.normal[i], obj_model.normal[i + 1], obj_model.normal[i + 2]);
                glm::vec2 uv(0.0f, 0.0f);
                if (obj_model.texCoord.size() > 0) {
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

    std::shared_ptr<Texture2D> Assets::texture(std::string file_name) {
        using namespace mo;
        if (textures_.find(file_name) == textures_.end()) {
            vector<unsigned char> texels_decoded;
            unsigned width, height;
#ifdef __ANDROID__
            std::cout << "Loading: " << file_name << std::endl;
            AAsset* asset = AAssetManager_open(manager_, file_name.c_str(), AASSET_MODE_UNKNOWN);
            int size = (int) AAsset_getLength(asset);

            const unsigned char * texels_encoded = static_cast<const unsigned char *> (AAsset_getBuffer(asset));

            unsigned error = lodepng::decode(texels_decoded, width, height, texels_encoded, size);
            if (error) {
                std::cout << "Decoder error: " << error << ": " << lodepng_error_text(error) << std::endl;
            }
#else
            std::cout << "Loading: " << directory_ + file_name << std::endl;
            unsigned error = lodepng::decode(texels_decoded, width, height, directory_ + file_name);
            if (error) {
                std::cout << "Decoder error: " << error << ": " << lodepng_error_text(error) << std::endl;
            }
#endif
            textures_.insert(TexturePair(file_name, std::make_shared<Texture2D>(Texture2D(texels_decoded.begin(), texels_decoded.end(), width, height))));

        }
        return textures_.at(file_name);
    }

    std::shared_ptr<Sound> Assets::sound(std::string file_name) {
        if (sounds_.find(file_name) == sounds_.end()) {
            int channels, length;
            short * decoded;
#ifdef __ANDROID__
            AAsset* asset = AAssetManager_open(manager_, file_name.c_str(), AASSET_MODE_UNKNOWN);
            int size = (int) AAsset_getLength(asset);

            length = stb_vorbis_decode_memory((unsigned char *) AAsset_getBuffer(asset), size, &channels, &decoded);
#else
            std::ifstream file(directory_ + file_name, std::ios::binary);
            std::vector<char> data;

            char v;
            while (file.read(&v, sizeof (v))) {
                data.push_back(v);
            }
            length = stb_vorbis_decode_memory((unsigned char *) data.data(), data.size(), &channels, &decoded);
#endif

            sounds_.insert(SoundPair(file_name, std::make_shared<Sound>(Sound(((char *) decoded), ((char*) decoded) + length))));
            return sounds_.at(file_name);
        } else {
            return sounds_.at(file_name);
        }
    }

    std::string Assets::text(std::string file_name) {
#ifdef __ANDROID__
        AAsset* text = AAssetManager_open(manager_, file_name.c_str(), AASSET_MODE_UNKNOWN);
        long size = AAsset_getLength(text);
        char * buffer = new char[size + 1];
        buffer[size] = 0;
        AAsset_read(text, buffer, size);

        return std::string(buffer, buffer + size);
#else  
        std::ifstream file(directory_ + file_name);
        std::string source((std::istreambuf_iterator<char>(file)),
                std::istreambuf_iterator<char>());
        return source;
#endif
    }

    std::map<char, Character> Assets::characterMap(std::string file_name) {

        std::map<char, Character> characters;
        rapidxml::xml_document<> doc;
        //doc.parse<0>((char*) text(path));
        std::string str = text(file_name);
        char* cstr = new char[str.size() + 1]; // Create char buffer to store string copy
        strcpy(cstr, str.c_str());
        doc.parse<0>(cstr);

        delete [] cstr;

        std::cout << "Name of my first node is: " << doc.first_node()->name() << "\n";

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
