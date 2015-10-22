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
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace mos {
	using namespace std;
	using namespace glm;
	
	Assets::Assets(const std::string directory) :
    directory_(directory) {    
    }

    Assets::~Assets() {
    }

    Model Assets::model(rapidjson::Value &value ) {
        std::string mesh = !value.HasMember("mesh") || value["mesh"].IsNull() ? "": value["mesh"].GetString();

        bool selectable =
                !(!value.HasMember("selectable") || value["selectable"].IsNull()) && value["selectable"].GetBool();

        bool step = !value.HasMember("step") || value["step"].IsNull() ? false: value["step"].GetBool();

        std::string texture_name = !value.HasMember("texture") || value["texture"].IsNull() ? "" : value["texture"].GetString();
        std::string texture2_name = !value.HasMember("texture2") || value["texture2"].IsNull() ? "" : value["texture2"].GetString();
        std::string lightmap_name = !value.HasMember("lightmap") || value["lightmap"].IsNull() ? "" : value["lightmap"].GetString();
        std::string material_name = !value.HasMember("material") || value["material"].IsNull() ? "" : value["material"].GetString();

        float obstruction = !value.HasMember("obstruction") || value["obstruction"].IsNull() ? 0.0f : value["obstruction"].GetDouble();

        glm::mat4 transform;

        if (value.HasMember("transform")){
            std::vector<float> nums;
            for (auto it = value["transform"].Begin(); it != value["transform"].End(); it++){
                nums.push_back(it->GetDouble());
            }
            transform = glm::make_mat4x4(nums.data());
            //std::cout << transform;
        }

            float x = value["position"][0].GetDouble();
            float y = value["position"][1].GetDouble();
            float z = value["position"][2].GetDouble();

            glm::vec3 axis(value["axis"][0].GetDouble(), value["axis"][1].GetDouble(), value["axis"][2].GetDouble());
            float angle = value["angle"].GetDouble();

            glm::mat4 transform2 = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)) * glm::rotate(glm::mat4(1.0f), angle, axis);

        auto m =  mos::Model(mesh_cached(mesh),
                         texture_cached(texture_name),
                         texture_cached(texture2_name),
                         transform,
                         mos::Model::Draw::TRIANGLES,
                         material_cached(material_name),
                         texture_cached(lightmap_name),
                         nullptr,
                         selectable,
                         step,
                         obstruction);

        for (auto it = value["models"].Begin(); it != value["models"].End(); it++) {
            m.models.push_back(model(*it));

        }
        m.transform(transform);
        return m;
    }

    rapidjson::Document Assets::document(const std::string & file_name) {
        //std::cout << "Loading: " << file_name << std::endl;
        std::ifstream is(directory_ + file_name);
        if (!is.good()){
            throw std::runtime_error(directory_ + file_name + " does not exist.");
        }
        std::ifstream file(directory_ + file_name);
        std::string source((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
        rapidjson::Document doc;
        doc.Parse(source.c_str());
        return doc;
    }

    vec3 Assets::position(const string & file_name) {
        return (glm::vec3)(transform(file_name)*glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    }

    mat4 Assets::transform(const string &file_name) {
        auto doc = document(file_name);
        glm::mat4 transform;
        if (doc.HasMember("transform")){
            std::vector<float> nums;
            for (auto it = doc["transform"].Begin(); it != doc["transform"].End(); it++){
                nums.push_back(it->GetDouble());
            }
            transform = glm::make_mat4x4(nums.data());
        }
        return transform;
    }

    Model Assets::model(const std::string file_name) {
        auto doc = document(file_name);
        return model(doc);
    }

    std::shared_ptr<Mesh> Assets::mesh(const std::string file_name) const {
        //TODO: allow Null mesh? As of now it becomes empty.
        vector<mos::Vertex> vertices;
        vector<int> indices;

        if (file_name.substr(file_name.find_last_of(".") + 1) == "mesh") {

            //std::cout << "Loading: " << file_name << std::endl;
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

            //std::cout << "Loading: " << directory_ << file_name;
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

        if (file_name.empty()) {
            return std::shared_ptr<Texture2D>(nullptr);
        }

        //std::cout << "Loading: " << directory_ + file_name << std::endl;
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
        int channels, length, sample_rate;
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
        length = stb_vorbis_decode_memory(data.data(), data.size(), &channels, &sample_rate, &decoded);

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
           // std::cout << "Loading: " << directory_ + file_name << std::endl;
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


    void Assets::clear_unused() {
        for (auto it = textures_.begin(); it != textures_.end();) {
            if(it->second.use_count() <= 1) {
                textures_.erase(it++);
            }
            else {
                ++it;
            }
        }
        for (auto it = meshes_.begin(); it != meshes_.end();) {
            if(it->second.use_count() <= 1) {
                meshes_.erase(it++);
            }
            else{
                ++it;
            }
        }
        for (auto it = sounds_.begin(); it != sounds_.end();) {
            if(it->second.use_count() <= 1) {
                sounds_.erase(it++);
            }
            else {
                ++it;
            }
        }
        for (auto it = materials_.begin(); it != materials_.end();) {
            if(it->second.use_count() <= 1) {
                materials_.erase(it++);
            } else {
                ++it;
            }
        }
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
