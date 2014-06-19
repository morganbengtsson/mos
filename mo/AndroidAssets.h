/* 
 * File:   AndroidAssets.h
 * Author: morgan
 *
 * Created on June 12, 2014, 2:29 PM
 */

#ifdef __ANDROID__

#ifndef MO_ANDROIDASSETS_H
#define	MO_ANDROIDASSETS_H

#include <mo/Assets.h>
#include <android/asset_manager.h>

namespace mo {

    class AndroidAssets : public Assets {
    public:
        AndroidAssets(AAssetManager * manager);        
        virtual ~AndroidAssets();
        
        std::shared_ptr<Mesh> mesh(std::string path);
        std::shared_ptr<Texture2D> texture(std::string path);
        std::shared_ptr<Sound> sound(std::string path);
        std::string text(std::string path);
    private:
        AAssetManager * manager_;
    };
}

#endif	/* ANDROIDASSETS_H */
#endif