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
             
        std::shared_ptr<Mesh> mesh(std::string file_name);  
        std::shared_ptr<Texture2D> texture(std::string file_name);
        std::shared_ptr<Sound> sound(std::string file_name);
        std::string text(std::string file_name);
        std::map<char, Character> characterMap(std::string path);

    private:
        std::string directory_;
        
    };
}
#endif	/* MO_FILEASSETS_H */

