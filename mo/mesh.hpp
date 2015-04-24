/* 
 * File:   Model.h
 * Author: morgan
 *
 * Created on February 25, 2014, 6:40 PM
 */

#ifndef MO_MESH_H
#define	MO_MESH_H

#include <vector>
#include "vertex.hpp"
#include "range.hpp"

namespace mo {

/*!
 * A class that describes the geometric data to be rendered. Contains vertices
 * and elements, for vertex order, when rendering.
 */
class Mesh {
public:
    typedef std::vector<Vertex> Vertices;
    typedef std::vector<int> Elements;

    template<class VerticesIt, class ElementsIt>
    /*!
     * \brief Mesh constructor
     * \param verticesBegin
     * \param verticesEnd
     * \param elementsBegin
     * \param elementsEnd
     *
     * Mesh constructor that takes Vertices and elements (vertex order for rendering).
     */
    Mesh(const VerticesIt vertices_begin,
         const VerticesIt vertices_end,
         ElementsIt elements_begin,
         ElementsIt elements_end) :
        valid(true),
        vertices_(vertices_begin, vertices_end),
        elements_(elements_begin, elements_end){
        id_ = current_id++;
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

    Vertices::const_iterator vertices_begin() const {
        return vertices_.begin();
    }

    Vertices::const_iterator vertices_end() const {
        return vertices_.end();
    }

    Elements::const_iterator elements_begin() const {
        return elements_.begin();
    }

    Elements::const_iterator elements_end() const {
        return elements_.end();
    }

    Vertices::iterator vertices_begin() {
        return vertices_.begin();
    }

    Vertices::iterator vertices_end() {
        return vertices_.end();
    }

    Elements::iterator elements_begin() {
        return elements_.begin();
    }

    Elements::iterator elements_end() {
        return elements_.end();
    }

    /**
     *
     * @return A unique identifier.
     */
    unsigned int id() const;

    bool valid;

    void clear();
    void add(const Vertex vertex);
    void add(const int element);
    const Vertex * vertices_data() const;
    unsigned int vertices_size() const;
    const int * elements_data() const;
    unsigned int elements_size() const;
private:
    static unsigned int current_id;
    unsigned int id_;
    Vertices vertices_;
    Elements elements_;
};

}

#endif	/* MO_MESH_H */

