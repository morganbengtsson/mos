/* 
 * File:   Assets.h
 * Author: morgan
 *
 * Created on February 25, 2014, 6:38 PM
 */

#ifndef MO_ASSETS_H
#define	MO_ASSETS_H

#include <string>
#include <map>
#include <memory>
#include "Mesh.h"
#include "Texture2D.h"

namespace mo {
    class Assets {
    public:
        typedef std::map<std::string, std::shared_ptr<mo::Mesh>> MeshMap;
        typedef std::map<std::string, std::shared_ptr<mo::Texture2D>> TextureMap;
        typedef std::pair<std::string, std::shared_ptr<mo::Mesh>> MeshPair;
        typedef std::pair<std::string, std::shared_ptr<mo::Texture2D>> TexturePair;

        Assets();
        virtual ~Assets();

        std::shared_ptr<Mesh> mesh(std::string path);
        std::shared_ptr<Texture2D> texture(std::string path);

    private:
        MeshMap models_;
        TextureMap textures_;
    };
}
#endif	/* ASSETS_H */

