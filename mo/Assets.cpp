/* 
 * File:   Assets.cpp
 * Author: morgan
 * 
 * Created on February 25, 2014, 6:38 PM
 */

#include "Assets.h"

#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>
#include <objload.h>
#include <lodepng.h>
#include <lodepng.cpp>
#include <exception>
#include <system_error>

namespace mo {

    Assets::Assets() {
        using namespace mo;
        using namespace std;

        std::vector<Vertex> vertices;
        std::vector<int> elements;
        models_.insert(MeshPair("Empty.obj", Mesh(vertices.begin(),
                vertices.end(),
                elements.begin(),
                elements.end())));
    }

    Assets::~Assets() {
    }

    const mo::Mesh & Assets::mesh(std::string path) {
        using namespace std;
        using namespace mo;

        if (models_.find(path) == models_.end()) {
            std::vector<mo::Vertex> vertices;
            std::vector<int> elements;

            std::cout << "Opening: " << path << std::endl;

            obj::Model obj_model = obj::loadModelFromFile(path);
            int j = 0;
            for (int i = 0; i < obj_model.vertex.size(); i += 3) {
                glm::vec3 position(obj_model.vertex[i], obj_model.vertex[i + 1], obj_model.vertex[i + 2]);
                glm::vec3 normal(obj_model.normal[i], obj_model.normal[i + 1], obj_model.normal[i + 2]);
                glm::vec2 uv(obj_model.texCoord[j], -obj_model.texCoord[j + 1]);
                j += 2;
                vertices.push_back(Vertex(position, normal, uv));
            }
            elements.assign(obj_model.faces.find("default")->second.begin(),
                    obj_model.faces.find("default")->second.end());

            models_.insert(MeshPair(path, mo::Mesh(vertices.begin(),
                    vertices.end(),
                    elements.begin(),
                    elements.end())));
            return models_.at(path);
        } else {
            return models_.at(path);
        }
    }

    const mo::Texture2D & Assets::texture(std::string path) {
        using namespace mo;
        if (textures_.find(path) == textures_.end()) {
            std::vector<unsigned char> texels;
            unsigned width, height;
            std::cout << "Opening: " << path << std::endl;
            unsigned error = lodepng::decode(texels, width, height, path);
            if (error) {
                std::cout << "Decoder error: " << error << ": " << lodepng_error_text(error) << std::endl;
                
            }
            textures_.insert(TexturePair(path, Texture2D(texels.begin(), texels.end(), width, height)));
            return textures_.at(path);
        } else {
            return textures_.at(path);
        }
    }
}
