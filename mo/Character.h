/* 
 * File:   Character.h
 * Author: morgan
 *
 * Created on July 15, 2014, 1:43 PM
 */

#ifndef MO_CHARACTER_H
#define	MO_CHARACTER_H

namespace mo {

    struct Character {
        float offsetX;
        float offsetY;
        float advance;
        float rectW;
        char id;
        float rectX;
        float rectY;
        float rectH;
    };
}

#endif	/* MO_CHARACTER_H */

