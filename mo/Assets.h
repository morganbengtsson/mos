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

        std::shared_ptr<Mesh> mesh(std::string file_name);
        std::shared_ptr<Texture2D> texture(std::string file_name);
        std::shared_ptr<Sound> sound(std::string file_name);
        std::string text(std::string file_name);
        std::map<char, Character> characterMap(std::string path);
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

