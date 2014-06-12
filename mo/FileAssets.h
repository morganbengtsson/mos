/* 
 * File:   Assets.h
 * Author: morgan
 *
 * Created on February 25, 2014, 6:38 PM
 */

#ifndef MO_FILEASSETS_H
#define	MO_FILEASSETS_H

#include "Assets.h"

namespace mo {
    class FileAssets : public Assets {
    public:        
        FileAssets();
        virtual ~FileAssets();
        
        /**
         * Load a mesh from file
         * 
         * Loads a mesh from an *.obj file, and caches it internally.
         * @param File path
         * @return Shared pointer to cached Mesh object. 
         */
        std::shared_ptr<Mesh> mesh(std::string path);
        
        /**
         * Load a texture from file.
         * 
         * Loads a texture from a PNG image file, and caches it internaly.
         * @param Path to the file
         * @return Shared pointer to cached Texture2D object
         */
        std::shared_ptr<Texture2D> texture(std::string path);
        /**
         * Loads sound from a file.
         * 
         * Loads a sound buffer from a raw data file, containing 16 bit integer data. Cached internally.
         * @param The file path
         * @return Shared pointer to cached Sound object
         */
        std::shared_ptr<Sound> sound(std::string path);

    private:
        MeshMap models_;
        TextureMap textures_;
        SoundMap sounds_;
    };
}
#endif	/* MO_FILEASSETS_H */

