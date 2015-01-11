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

    Vertex(const glm::vec3 position = glm::vec3(0.0f),
           const glm::vec3 normal = glm::vec3(0.0f),
           const glm::vec2 uv = glm::vec2(0.0f),
           const glm::vec2 uv_lightmap = glm::vec2(0.0f))
        : position(position),
          normal(normal),
          uv(uv),
          uv_lightmap(uv_lightmap) {
    }
    ~Vertex(){
    }

    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec2 uv_lightmap;
};

}

#endif	/* MO_VERTEX_H */

