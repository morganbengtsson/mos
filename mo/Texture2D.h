/* 
 * File:   Texture.h
 * Author: morgan
 *
 * Created on February 25, 2014, 7:26 PM
 */

#ifndef OGLI_TEXTURE2D_H
#define	OGLI_TEXTURE2D_H

#include <vector>

namespace mo {

class Texture2D {
public:
    typedef std::vector<unsigned char> Texels;
    
    template<class It>
    Texture2D(It begin, It end, unsigned int width, unsigned int height) : width_(width), height_(height){
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
    
private:
    unsigned int id_;
    Texels texels_;
    unsigned int width_;
    unsigned int height_;
};

}

#endif	/* OGLI_TEXTURE_H */

