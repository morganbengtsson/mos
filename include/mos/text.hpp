#ifndef MOS_TEXT_H
#define	MOS_TEXT_H

#include "model.hpp"
#include "mesh.hpp"
#include "character.hpp"

#include <map>
#include <memory>
#include <string>

namespace mos {

/**
 * A special Model class for text rendering. 
 */
class Text : public Model {
public:
    /**
     * @brief Text constructor
     * @param text
     * @param characters
     * @param texture with characters
     * @param transform for position/rotation
     * @param spacing between characters
     */
    Text(const std::string text, 
            std::map<char, Character> characters, 
            std::shared_ptr<Texture2D> texture, 
            const glm::mat4 transform = glm::mat4(1.0f), 
            const float spacing = 0.0f);
    virtual ~Text();    

    /**
     * Update the text.
     * @param text String to use.
     */
    void text(const std::string text);

    /**
     * @brief text
     * @return Current text
     */
    std::string text() const;

    /**
     * @brief approximate width
     * @return width
     */
    float width();

    /**
     * @brief spacing between characters
     */
    float spacing;
private:   
    std::string text_;
    std::map<char, Character> characters_;
   
};
}

#endif	/* MOS_TEXT_H */

