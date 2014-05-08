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
#include "Sound.h"

namespace mo {
    class Assets {
    public:
        typedef std::map<std::string, std::shared_ptr<mo::Mesh>> MeshMap;
        typedef std::map<std::string, std::shared_ptr<mo::Texture2D>> TextureMap;
        typedef std::pair<std::string, std::shared_ptr<mo::Mesh>> MeshPair;
        typedef std::pair<std::string, std::shared_ptr<mo::Texture2D>> TexturePair;
        typedef std::map<std::string, std::shared_ptr<mo::Sound>> SoundMap;
        typedef std::pair<std::string, std::shared_ptr<mo::Sound>> SoundPair;

        Assets();
        virtual ~Assets();

        std::shared_ptr<Mesh> mesh(std::string path);
        
        /**
         * Load a texture from file.
         * 
         * Loads a texture from a PNG image file.
         * @param Path to the file
         * @return Shared pointer to a Texture2D object
         */
        std::shared_ptr<Texture2D> texture(std::string path);
        /**
         * Load sound from file.
         * 
         * Load a sound buffer from a raw data file, containing 16 bit integer data.
         * @param The file path
         * @return Shared pointer to a Sound object
         */
        std::shared_ptr<Sound> sound(std::string path);

    private:
        MeshMap models_;
        TextureMap textures_;
        SoundMap sounds_;
    };
}
#endif	/* ASSETS_H */

