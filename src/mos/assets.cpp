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
#include <memory>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stb_vorbis.h>
#include <glm/gtx/io.hpp>

namespace mos {
	using namespace std;
	using namespace glm;
	
	Assets::Assets(const std::string directory) :
    directory_(directory) {    
    }

    Assets::~Assets() {		
    }

    std::shared_ptr<Mesh> Assets::mesh(const std::string file_name) const {
        //TODO: allow Null mesh? As of now it becomes empty.
        vector<mos::Vertex> vertices;
        vector<int> indices;

        if (file_name.substr(file_name.find_last_of(".") + 1) == "mesh") {

            std::cout << "Loading: " << file_name << std::endl;
            std::ifstream is(directory_ + file_name, ios::binary);
            if (!is.good()){
                throw std::runtime_error(directory_ + file_name + " does not exist.");
            }
            int num_vertices;
            int num_indices;
		    is.read((char*) &num_vertices, sizeof (int));
            is.read((char*) &num_indices, sizeof (int));
		
			vertices = vector<mos::Vertex>(num_vertices);
            indices = vector<int>(num_indices);
			
			if (vertices.size() > 0){
				is.read((char*)&vertices[0], vertices.size() * sizeof(Vertex));
			}
            
			if (indices.size() > 0) {
				is.read((char*)&indices[0], indices.size() * sizeof(int));
			}

        } else if (file_name.substr(file_name.find_last_of(".") + 1) == "obj"){

            std::cout << "Loading: " << directory_ << file_name;
            obj::Model obj_model = obj::loadModelFromFile(directory_ + file_name);

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
        return std::make_shared<Mesh>(vertices.begin(),
                vertices.end(),
                indices.begin(),
                indices.end());
    }

    std::shared_ptr<Mesh> Assets::mesh_cached(const std::string file_name) {		
		if (meshes_.find(file_name) == meshes_.end()) {
            meshes_.insert(MeshPair(file_name, mesh(file_name)));
        }
        return meshes_.at(file_name);
    }

    std::shared_ptr<Texture2D> Assets::texture(const std::string file_name, const bool mipmaps) const {
        using namespace mos;

        vector<unsigned char> texels_decoded;
        unsigned width, height;

        std::cout << "Loading: " << directory_ + file_name << std::endl;
        auto error = lodepng::decode(texels_decoded, width, height, directory_ + file_name);
        if (error) {
            std::cout << "Decoder error: " << error << ": " << lodepng_error_text(error) << std::endl;
        }
        return std::make_shared<Texture2D>(texels_decoded.begin(), texels_decoded.end(), width, height, mipmaps);
    }

    std::shared_ptr<Texture2D> Assets::texture_cached(const std::string file_name, const bool mipmaps) {
        if (!file_name.empty()) {
            if (textures_.find(file_name) == textures_.end()) {
                textures_.insert(TexturePair(file_name, texture(file_name, mipmaps)));
            }
            return textures_.at(file_name);
        }
        else {
            return std::shared_ptr<Texture2D>(nullptr);
        }
    }

    std::shared_ptr<Sound> Assets::sound(const std::string file_name) const {
        int channels, length;
        short * decoded;

        std::ifstream file(directory_ + file_name, std::ios::binary);
        if (!file.good()){
            throw std::runtime_error(directory_ + file_name + " does not exist.");
        }
        std::vector<unsigned char> data;

        unsigned char c;
        while (file.read(reinterpret_cast<char*> (&c), sizeof (c))) {
            data.push_back(c);
        }
        length = stb_vorbis_decode_memory(data.data(), data.size(), &channels, &decoded);

        return std::make_shared<Sound>(decoded, decoded + length);
    }

    std::shared_ptr<Stream> Assets::stream(const string file_name) const {
		return std::make_shared<mos::Stream>(directory_ + file_name);
    }

    std::shared_ptr<Sound> Assets::sound_cached(const std::string file_name) {
        if (sounds_.find(file_name) == sounds_.end()) {

            sounds_.insert(SoundPair(file_name, sound(file_name)));
            return sounds_.at(file_name);
        } else {
            return sounds_.at(file_name);
        }
    }

    std::shared_ptr<Material> Assets::material(const std::string file_name) const{

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
			 

            return std::make_shared<Material>(ambient, diffuse, specular,
                    opacity, specular_exponent);
        } else {
            //throw std::runtime_error(file_name.substr(file_name.find_last_of(".")) + " file format is not supported.");
            return std::shared_ptr<Material>(nullptr);
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

        std::ifstream file(directory_ + file_name);
        std::string source((std::istreambuf_iterator<char>(file)),
                std::istreambuf_iterator<char>());
        return source;
    }

    std::map<char, Character> Assets::character_map(std::string file_name) {

        std::map<char, Character> characters;
        rapidxml::xml_document<> doc;
        auto str = text(file_name);
        char* cstr = new char[str.size() + 1]; // Create char buffer to store string copy
        std::strcpy(cstr, str.c_str());
        doc.parse<0>(cstr);

        rapidxml::xml_node<> * chars_node = doc.first_node("font")->first_node("chars");

        for (rapidxml::xml_node<> * char_node = chars_node->first_node("char");
                char_node;
                char_node = char_node->next_sibling()) {

            Character character;
            rapidxml::xml_attribute<> *attr = char_node->first_attribute();
            character.offset_x = atof(attr->value());
            attr = attr->next_attribute();
            character.offset_y = atof(attr->value());
            attr = attr->next_attribute();
            character.advance = atof(attr->value());
            attr = attr->next_attribute();
            character.rect_w = atof(attr->value());
            attr = attr->next_attribute();
            character.id = *attr->value();
            attr = attr->next_attribute();
            character.rect_x = atof(attr->value());
            attr = attr->next_attribute();
            character.rect_y = atof(attr->value());
            attr = attr->next_attribute();
            character.rect_h = atof(attr->value());

            characters.insert(std::pair<char, Character>(character.id, character));

        }
        delete [] cstr;
        return characters;
    }
}
