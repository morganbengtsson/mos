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
        typedef std::map<std::string, mo::Mesh> MeshMap;
        typedef std::map<std::string, mo::Texture2D> TextureMap;
        typedef std::pair<std::string, mo::Mesh> MeshPair;
        typedef std::pair<std::string, mo::Texture2D> TexturePair;

        Assets();
        virtual ~Assets();

        const mo::Mesh & mesh(std::string path);
        const mo::Texture2D & texture(std::string path);

    private:
        MeshMap models_;
        TextureMap textures_;
    };
}
#endif	/* ASSETS_H */

