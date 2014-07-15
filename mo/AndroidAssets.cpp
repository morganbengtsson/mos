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
                glm::vec2 uv(obj_model.texCoord[j], -obj_model.texCoord[j + 1]);
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
        }       
        return textures_.at(file_name);
    }

    std::shared_ptr<Sound> AndroidAssets::sound(std::string file_name) {
        return 0;
    }
    
    std::string AndroidAssets::text(std::string file_name){
        AAsset* text = AAssetManager_open(manager_, file_name.c_str(), AASSET_MODE_UNKNOWN);
        long size = AAsset_getLength(text);
        char * buffer = new char[size + 1];
        buffer[size] = 0;
        AAsset_read(text, buffer, size);
        
        return std::string(buffer, buffer + size);
    }
}
#endif



