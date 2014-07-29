/* 
 * File:   Text.h
 * Author: morgan
 *
 * Created on August 28, 2013, 7:49 PM
 */

#ifndef MO_TEXT_H
#define	MO_TEXT_H

#include "Model.h"
#include "Mesh.h"
#include "Character.h"

#include <map>
#include <memory>

namespace mo {

/*!
 * A special Model class for text rendering. 
 */
class Text : public Model {
public:
    Text(const std::string text, std::map<char, Character> characters, std::shared_ptr<Texture2D> texture, const glm::mat4 transform = glm::mat4(1.0f));
    virtual ~Text();    

    /**
     * Update the text.
     * @param Text.
     */
    void setText(const std::string text);
    
private:   
    std::string text_;
    std::map<char, Character> characters_;
   
};
}

#endif	/* TEXT_H */

