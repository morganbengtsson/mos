/* 
 * File:   Character.h
 * Author: morgan
 *
 * Created on July 15, 2014, 1:43 PM
 */

#ifndef MO_CHARACTER_H
#define	MO_CHARACTER_H

namespace mo {

    /*!
     * Describes measurements of a character. Used by the Text object
     * for text rendering.
     */
    struct Character {
        float offset_x;
        float offset_y;
        float advance;
        float rect_w;
        char id;
        float rect_x;
        float rect_y;
        float rect_h;
    };
}

#endif	/* MO_CHARACTER_H */

