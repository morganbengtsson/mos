/* 
 * File:   Vertex.h
 * Author: morgan
 *
 * Created on November 21, 2013, 8:16 PM
 */

#ifndef MO_VERTEX_H
#define	MO_VERTEX_H

#include <glm/glm.hpp>

namespace mos {

/*!
 * The vertex structure, supported by the renderer.
 */
class Vertex {
public:

    /*!
     * @brief Vertex constructor
     * @param position
     * @param normal
     * @param uv
     * @param uv_lightmap
     *
     * Constructs a vertex. Probably the smallest primitive in this context.
     *
     */
    explicit Vertex(const glm::vec3 position = glm::vec3(0.0f),
           const glm::vec3 normal = glm::vec3(0.0f),
           const glm::vec2 uv = glm::vec2(0.0f),
           const glm::vec2 uv_lightmap = glm::vec2(0.0f));

    explicit Vertex(const float p1, const float p2, const float p3,
                    const float n1 = 0.0f, const float n2 = 0.0f, const float n3 = 0.0f,
                    const float u1 = 0.0f, const float u2 = 0.0f,
                    const float ul1 = 0.0f, const float ul2 = 0.0f);
    ~Vertex();

    Vertex operator+(const Vertex & vertex) const;
    Vertex operator-(const Vertex & vertex) const;
    Vertex operator*(const Vertex & vertex) const;
    Vertex operator*(const float number) const;
    Vertex operator/(const float number) const;
    Vertex operator/(const Vertex & vertex) const;

    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec2 uv_lightmap;
};

}

#endif	/* MO_VERTEX_H */

