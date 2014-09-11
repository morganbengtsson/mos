/* 
 * File:   Texture.h
 * Author: morgan
 *
 * Created on February 25, 2014, 7:26 PM
 */

#ifndef MO_TEXTURE2D_H
#define	MO_TEXTURE2D_H

#include <vector>

namespace mo {

/*!
 * 2D texture.
 */
class Texture2D {
public:
    typedef std::vector<unsigned char> Texels;
    
    template<class It>
    Texture2D(It begin, It end, unsigned int width, unsigned int height, const bool mipmaps = true) : width_(width), height_(height), mipmaps(mipmaps){
        static unsigned int current_id = 0;
        id_ = current_id++;
        texels_.assign(begin, end);
    }
    virtual ~Texture2D();
    
    typename Texels::const_iterator begin() const { return texels_.begin();};
    typename Texels::const_iterator end() const { return texels_.end();};
    
    const unsigned int id() const;
    
    unsigned int width() const;
    unsigned int height() const;
    
    bool mipmaps;
private:
    unsigned int id_;
    Texels texels_;
    unsigned int width_;
    unsigned int height_;
};

}

#endif	/* MO_TEXTURE_H */

