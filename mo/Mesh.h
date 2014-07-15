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
        Mesh(const VerticesIt verticesBegin,
                const VerticesIt verticesEnd,
                ElementsIt elementsBegin,
                ElementsIt elementsEnd) {
            static unsigned int current_id = 0;
            id_ = current_id++;
            vertices_.assign(verticesBegin, verticesEnd);
            elements_.assign(elementsBegin, elementsEnd);
        }
        Mesh();
        virtual ~Mesh();

        typename Vertices::const_iterator verticesBegin() const {
            return vertices_.begin();
        };

        typename Vertices::const_iterator verticesEnd() const {
            return vertices_.end();
        };

        typename Elements::const_iterator elementsBegin() const {
            return elements_.begin();
        };

        typename Elements::const_iterator elementsEnd() const {
            return elements_.end();
        };

        unsigned int id() const;
        
        void clear();
        void add(const Vertex vertex);
        void add(const int element);

    private:
        unsigned int id_;
        Vertices vertices_;
        Elements elements_;

    };

}

#endif	/* MESH_H */

