/* 
 * File:   Text.h
 * Author: morgan
 *
 * Created on August 28, 2013, 7:49 PM
 */

#ifndef MO_TEXT_H
#define	MO_TEXT_H

#include "model.hpp"
#include "mesh.hpp"
#include "character.hpp"

#include <map>
#include <memory>
#include <string>

namespace mo {

/*!
 * A special Model class for text rendering. 
 */
class Text : public Model {
public:
    /*!
     * \brief Text constructor
     * \param text
     * \param characters
     * \param texture with characters
     * \param transform for position/rotation
     * \param spacing between characters
     */
    Text(const std::string text, 
            std::map<char, Character> characters, 
            std::shared_ptr<Texture2D> texture, 
            const glm::mat4 transform = glm::mat4(1.0f), 
            const float spacing = 0.0f);
    virtual ~Text();    

    /*!
     * Update the text.
     * \param Text.
     */
    void text(const std::string text);

    /*!
     * \brief text
     * \return current text
     */
    std::string text() const;

    /*!
     * \brief approximate width
     * \return width
     */
    float width();

    /*!
     * \brief spacing between characters
     */
    float spacing;
private:   
    std::string text_;
    std::map<char, Character> characters_;
   
};
}

#endif	/* TEXT_H */

