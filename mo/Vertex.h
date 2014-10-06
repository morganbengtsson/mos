/* 
 * File:   Vertex.h
 * Author: morgan
 *
 * Created on November 21, 2013, 8:16 PM
 */

#ifndef MO_VERTEX_H
#define	MO_VERTEX_H

#include <glm/glm.hpp>

namespace mo {

 /*!
  * The vertex structure, supported by the renderer.
  */
class Vertex {
public:
    Vertex(){};
    Vertex(const glm::vec3 & position, 
            const glm::vec3 & normal, 
            const glm::vec2 & uv):position(position), normal(normal), uv(uv){}
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};

}

#endif	/* MO_VERTEX_H */

