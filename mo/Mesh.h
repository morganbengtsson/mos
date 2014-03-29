/* 
 * File:   Model.h
 * Author: morgan
 *
 * Created on February 25, 2014, 6:40 PM
 */

#ifndef OGLI_MESH_H
#define	OGLI_MESH_H

#include <vector>
#include "Vertex.h"

namespace mo {

    class Mesh {
    public:
        typedef std::vector<Vertex> Vertices;
        typedef std::vector<int> Elements;

        template<class VerticesIt, class ElementsIt>
        Mesh(VerticesIt verticesBegin,
                VerticesIt verticesEnd,
                ElementsIt elementsBegin,
                ElementsIt elementsEnd) {
            static unsigned int current_id = 0;
            id_ = current_id++;
            vertices_.assign(verticesBegin, verticesEnd);
            elements_.assign(elementsBegin, elementsEnd);
        }
        virtual ~Mesh();

        typename Vertices::const_iterator verticesBegin() {
            return vertices_.begin();
        };

        typename Vertices::const_iterator verticesEnd() {
            return vertices_.end();
        };

        typename Elements::const_iterator elementsBegin() {
            return elements_.begin();
        };

        typename Elements::const_iterator elementsEnd() {
            return elements_.end();
        };

        const unsigned int id() const;

    private:
        unsigned int id_;
        Vertices vertices_;
        Elements elements_;

    };

}

#endif	/* MESH_H */

