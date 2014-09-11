/* 
 * File:   Assets.h
 * Author: morgan
 *
 * Created on February 25, 2014, 6:38 PM
 */


#ifndef MO_FILEASSETS_H
#define	MO_FILEASSETS_H

#include "Sound.h"
#include "Character.h"
#include "Mesh.h"
#include "Texture2D.h"
#include <map>
#include <memory>
#ifdef __ANDROID__
#include <android/asset_manager.h>
#endif

namespace mo {

    // File descriptor for streamed assets.
    struct Descriptor{
            int32_t descriptor;
            off_t start;
            off_t length;
    };
    
    /*!
     * An asset class that manages heavy resources such as Textures, meshes and sounds. 
     * Caches most things internally, so nothing is loaded twice. 
     */
    class Assets {
    public:
        typedef std::map<std::string, std::shared_ptr<mo::Mesh>> MeshMap;
        typedef std::map<std::string, std::shared_ptr<mo::Texture2D>> TextureMap;
        typedef std::pair<std::string, std::shared_ptr<mo::Mesh>> MeshPair;
        typedef std::pair<std::string, std::shared_ptr<mo::Texture2D>> TexturePair;
        typedef std::map<std::string, std::shared_ptr<mo::Sound>> SoundMap;
        typedef std::pair<std::string, std::shared_ptr<mo::Sound>> SoundPair;
#ifdef __ANDROID__
        Assets(AAssetManager * manager);
#else
        Assets();
#endif
        virtual ~Assets();

        /**
         * Loads a *.obj file into a mesh object, and caches it internally.
         * 
         * @param file_name
         * @return Shared pointer to Mesh object.
         */
        std::shared_ptr<Mesh> meshCached(const std::string file_name);
        
        /**
         * Loads a *.obj file into a mesh object. Not cached.
         * 
         * @param file_name
         * @return Shared pointer to Mesh object
         */        
        std::shared_ptr<Mesh> mesh(const std::string file_name) const;

        /**
         * Loads a *.png file into a Texture2D object, and caches it internally.
         * 
         * @param file_name
         * @return Shared pointer to Texture2D object.
         */
        std::shared_ptr<Texture2D> textureCached(const std::string file_name, const bool mipmaps = true);

        
        /**
         * Loads a *.png file into A Texture2D object. Not cached. 
         * 
         * @param file_name
         * @return Shared pointer to Texture2D object.
         */        
        std::shared_ptr<Texture2D> texture(const std::string file_name, const bool mipmaps = true) const;
        
        /**
         * Loads a *.ogg file into a Sound object, and caches it internally.
         * 
         * @param file_name
         * @return Shared pointer to Sound object.
         */
        std::shared_ptr<Sound> soundCached(const std::string file_name);

        
        /**
         * Loads an *. ogg file into a Sound object. Not cached.
         * 
         * @param file_name
         * @return Shared pointer to Sound object.
         */        
        std::shared_ptr<Sound> sound(const std::string file_name) const;
        
        
        /**
         * Load text from file. Not cached.
         * 
         * @param file_name
         * @return String.
         */
        std::string text(const std::string file_name) const;

        /**
         * Load a character map from an *.xml file in NGL format.
         * 
         * @param path
         * @return A map of chars with corresponding Character objects.
         */
        std::map<char, Character> characterMap(std::string path);
        
        /**
         * Descriptor for streaming of files.
         * @param path
         * @return 
         */
        Descriptor descript(std::string path) const;

    private:
        MeshMap models_;
        TextureMap textures_;
        SoundMap sounds_;
#ifdef __ANDROID__
        AAssetManager * manager_;
#else
        std::string directory_;
#endif



    };
}
#endif	/* MO_FILEASSETS_H */

