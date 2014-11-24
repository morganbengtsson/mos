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
#include "Range.h"


namespace mo {

    /*!
     * A class that describes the geometric data to be rendered. Contains vertices
     * and elements, describing vertex order.
     */
    class Mesh {
    public:
        typedef std::vector<Vertex> Vertices;
        typedef std::vector<int> Elements;

        template<class VerticesIt, class ElementsIt>
        Mesh(const VerticesIt verticesBegin,
                const VerticesIt verticesEnd,
                ElementsIt elementsBegin,
                ElementsIt elementsEnd) : valid(true) {
            id_ = current_id++;
            vertices_.assign(verticesBegin, verticesEnd);
            elements_.assign(elementsBegin, elementsEnd);
        }
        Mesh();
        Mesh(const Mesh & mesh);
        virtual ~Mesh();

        Range<Vertices> vertices() {
            return Range<Vertices>(vertices_);
        }
        
        Range<Elements> elements() {
            return Range<Elements>(elements_);
        }
        
        ConstRange<Vertices> vertices() const {
            return ConstRange<Vertices>(vertices_);
        }
        
        ConstRange<Elements> elements() const {
            return ConstRange<Elements>(elements_);
        }
        
        Vertices::const_iterator verticesBegin() const {
            return vertices_.begin();
        };

        Vertices::const_iterator verticesEnd() const {
            return vertices_.end();
        };

        Elements::const_iterator elementsBegin() const {
            return elements_.begin();
        };

        Elements::const_iterator elementsEnd() const {
            return elements_.end();
        };

        Vertices::iterator verticesBegin() {
            return vertices_.begin();
        };

        Vertices::iterator verticesEnd() {
            return vertices_.end();
        };

        Elements::iterator elementsBegin() {
            return elements_.begin();
        };

        Elements::iterator elementsEnd() {
            return elements_.end();
        };
        
        /**
         * 
         * @return A unique identifier. 
         */
        unsigned int id() const;

        bool valid;

        void clear();
        void add(const Vertex vertex);
        void add(const int element);
        Vertex back();

    private:
        static unsigned int current_id;
        unsigned int id_;
        Vertices vertices_;
        Elements elements_;

    };

}

#endif	/* MESH_H */

