/* 
 * File:   Assets.cpp
 * Author: morgan
 * 
 * Created on February 25, 2014, 6:38 PM
 */

#include "assets.hpp"

#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>
#include <lodepng.h>
#include <objload.h>
#include <exception>
#include <system_error>
#include <memory>
#include <fstream>
#include <cstring>
#include <iostream>
#include <iterator>
#include <stb_vorbis.h>
#include <algorithm>
#include <glm/gtx/io.hpp>

using namespace mo;
using namespace std;
using namespace glm;

namespace mo {

#ifdef __ANDROID__

    Assets::Assets(AAssetManager * manager) : manager_(manager) {
    }
#else

    Assets::Assets(const std::string directory) :
    directory_(directory) {
        /*
        vector<Vertex> vertices;
        vector<int> elements;
        models_.insert(MeshPair("Empty.obj", std::make_shared<Mesh>(Mesh(vertices.begin(),
                vertices.end(),
                elements.begin(),
                elements.end()))));*/
    }
#endif

    Assets::~Assets() {
    }

    std::shared_ptr<Mesh> Assets::mesh(const std::string file_name) const {
        vector<mo::Vertex> vertices;
        vector<int> indices;


        if (file_name.substr(file_name.find_last_of(".") + 1) == "mesh") {

            std::cout << "Loading: " << file_name << std::endl;
            std::ifstream is(directory_ + file_name, ios::binary);
            int num_vertices;
            int num_indices;
            is.read((char*) &num_vertices, sizeof (int));
            is.read((char*) &num_indices, sizeof (int));

            vertices = vector<mo::Vertex>(num_vertices);
            indices = vector<int>(num_indices);


            is.read((char*) &vertices[0], vertices.size() * sizeof (Vertex));
            is.read((char*) &indices[0], indices.size() * sizeof (int));

        } else {

#ifdef __ANDROID__
            LOGI("Loading: %s", file_name.c_str());
            std::string source = text(file_name);
            obj::Model obj_model = obj::loadModelFromString(source);
#else
            std::cout << "Loading: " << directory_ << file_name;
            obj::Model obj_model = obj::loadModelFromFile(directory_ + file_name);
#endif
            int j = 0;
            for (int i = 0; i < obj_model.vertex.size(); i += 3) {
                glm::vec3 position(obj_model.vertex[i], obj_model.vertex[i + 1], obj_model.vertex[i + 2]);
                glm::vec3 normal(obj_model.normal[i], obj_model.normal[i + 1], obj_model.normal[i + 2]);
                glm::vec2 uv(0.0f, 0.0f);
                if (obj_model.texCoord.size() > 0) {
                    uv.x = obj_model.texCoord[j];
                    uv.y = obj_model.texCoord[j + 1];
                }

                j += 2;
                vertices.push_back(Vertex(position, normal, uv));
            }
            indices.assign(obj_model.faces.find("default")->second.begin(),
                    obj_model.faces.find("default")->second.end());
        }
        /*
        std::cout << "Vertices:" << std::endl;
        for (auto v : vertices) {
            std::cout << "(" << v.position << v.uv << ")";
        }
        std::cout << "\nIndices\n";
        std::cout << std::endl;
        for (auto i : indices) {
            std::cout << i < " ";
        }
        std::cout << std::endl;*/
        return std::make_shared<Mesh>(mo::Mesh(vertices.begin(),
                vertices.end(),
                indices.begin(),
                indices.end()));
    }

    std::shared_ptr<Mesh> Assets::mesh_cached(const std::string file_name) {
        if (models_.find(file_name) == models_.end()) {
            models_.insert(MeshPair(file_name, mesh(file_name)));
        }
        return models_.at(file_name);
    }

    std::shared_ptr<Texture2D> Assets::texture(const std::string file_name, const bool mipmaps) const {
        using namespace mo;

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
        return std::make_shared<Texture2D>(Texture2D(texels_decoded.begin(), texels_decoded.end(), width, height, mipmaps));
    }

    std::shared_ptr<Texture2D> Assets::texture_cached(const std::string file_name, const bool mipmaps) {
        if (!file_name.empty()) {
            if (textures_.find(file_name) == textures_.end()) {
                textures_.insert(TexturePair(file_name, texture(file_name, mipmaps)));
            }
            return textures_.at(file_name);
        }
        else{
            return std::shared_ptr<Texture2D>(nullptr);
        }
    }

    std::shared_ptr<Sound> Assets::sound(const std::string file_name) const {
        int channels, length;
        short * decoded;
#ifdef __ANDROID__
        AAsset* asset = AAssetManager_open(manager_, file_name.c_str(), AASSET_MODE_UNKNOWN);
        int size = (int) AAsset_getLength(asset);

        length = stb_vorbis_decode_memory((unsigned char *) AAsset_getBuffer(asset), size, &channels, &decoded);
#else
        std::ifstream file(directory_ + file_name, std::ios::binary);
        std::vector<unsigned char> data;

        unsigned char c;
        while (file.read(reinterpret_cast<char*> (&c), sizeof (c))) {
            data.push_back(c);
        }
        length = stb_vorbis_decode_memory(data.data(), data.size(), &channels, &decoded);
#endif
        return std::make_shared<Sound>(Sound(decoded, decoded + length));
    }

    std::shared_ptr<Stream> Assets::stream(const string file_name) const
    {
        return std::shared_ptr<mo::Stream>(new mo::Stream(directory_ + file_name));
    }

    std::shared_ptr<Sound> Assets::sound_cached(const std::string file_name) {
        if (sounds_.find(file_name) == sounds_.end()) {

            sounds_.insert(SoundPair(file_name, sound(file_name)));
            return sounds_.at(file_name);
        } else {
            return sounds_.at(file_name);
        }
    }

    std::shared_ptr<Material> Assets::material(const std::string file_name) {

         if (file_name.substr(file_name.find_last_of(".") + 1) == "material") {
             std::cout << "Loading: " << directory_ + file_name << std::endl;
            std::ifstream is(directory_ + file_name, ios::binary);
            glm::vec3 ambient;
            glm::vec3 diffuse;
            glm::vec3 specular;
            float opacity;
            float specular_exponent;

            is.read((char*) &ambient, sizeof (glm::vec3));
            is.read((char*) &diffuse, sizeof (glm::vec3));
            is.read((char*) &specular, sizeof (glm::vec3));
            is.read((char*) &opacity, sizeof (float));
            is.read((char*) &specular_exponent, sizeof (float));

            //std::cout << ambient << std::endl;
            //std::cout << diffuse << std::endl;
            //std::cout << specular << std::endl;
            //std::cout << opacity << std::endl;
            //std::cout << specular_exponent << std::endl;

            return std::make_shared<Material>(Material(ambient, diffuse, specular,
                    opacity, specular_exponent));
        } else {
			 //TODO: parse obj material
			/*
            std::vector<tinyobj::material_t> materials;
            std::vector<tinyobj::shape_t> shapes;

            auto path = directory_ + file_name;

            std::string err = tinyobj::LoadObj(shapes, materials, path.c_str());
            if (!err.empty()) {
                std::cerr << err << std::endl;
                throw std::runtime_error("Error reading obj file.");
            }
            auto m = materials.front();
            Material material(glm::vec3(m.ambient[0], m.ambient[1], m.ambient[2]),
                    glm::vec3(m.diffuse[0], m.diffuse[1], m.diffuse[2]),
                    glm::vec3(m.specular[0], m.specular[1], m.specular[2]));
            return std::make_shared<Material>(material);
			*/
        }
    }

    std::shared_ptr<Material> Assets::material_cached(const std::string file_name) {
        if (materials_.find(file_name) == materials_.end()) {

            materials_.insert(MaterialPair(file_name, material(file_name)));
            return materials_.at(file_name);
        } else {
            return materials_.at(file_name);
        }
    }

    std::string Assets::text(const std::string file_name) const {
#ifdef __ANDROID__

        AAsset* text = AAssetManager_open(manager_, file_name.c_str(), AASSET_MODE_UNKNOWN);
        long size = (text == nullptr) ? 0 : AAsset_getLength(text);
        char * buffer = new char[size + 1];
        buffer[size] = 0;
        if (text != nullptr) {
            AAsset_read(text, buffer, size);
        }

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
        std::strcpy(cstr, str.c_str());
        doc.parse<0>(cstr);

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
        delete [] cstr;
        return characters;
    }

    Descriptor Assets::descript(std::string path) const {
#ifdef __ANDROID__
        Descriptor descriptor = {-1, 0, 0};
        AAsset* asset = AAssetManager_open(manager_, path.c_str(),
                AASSET_MODE_UNKNOWN);
        if (asset != NULL) {
            descriptor.descriptor = AAsset_openFileDescriptor(
                    asset, &descriptor.start, &descriptor.length);
            AAsset_close(asset);
        }
        return descriptor;
#else
        return Descriptor{-1, 0, 0};
#endif
    }


}
