/* 
 * File:   AndroidAssets.cpp
 * Author: morgan
 * 
 * Created on June 12, 2014, 2:29 PM
 */

#ifdef __ANDROID__

#include "AndroidAssets.h"
#include "Vertex.h"
#include <glm/glm.hpp>
#include <vector>



namespace mo {

    AndroidAssets::AndroidAssets(AAssetManager * manager) : manager_(manager) {
    }

    AndroidAssets::~AndroidAssets() {
    }

    std::shared_ptr<Mesh> AndroidAssets::mesh(std::string path) {
        using namespace std;
        using namespace glm;
        vector<Vertex> vertices = {Vertex(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f))};
        vector<int> elements = {0};
        shared_ptr<Mesh> mesh(new Mesh(vertices.begin(), vertices.end(), elements.begin(), elements.end()));
        return mesh;
    }

    std::shared_ptr<Texture2D> AndroidAssets::texture(std::string path) {
        using namespace std;
        using namespace glm;
        vector<unsigned char> texels = {1, 2, 3, 4};
        shared_ptr<Texture2D> texture(new Texture2D(texels.begin(), texels.end(), 2, 2));
        return texture;
    }

    std::shared_ptr<Sound> AndroidAssets::sound(std::string path) {
        return 0;
    }
    
    std::string AndroidAssets::text(std::string path){
        AAsset* text = AAssetManager_open(manager_, path.c_str(), AASSET_MODE_UNKNOWN);
        long size = AAsset_getLength(text);
        char * buffer = new char[size + 1];
        buffer[size] = 0;
        AAsset_read(text, buffer, size);
        
        return std::string(buffer, buffer + size);
    }
}
#endif



