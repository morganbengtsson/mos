/* 
 * File:   Model.h
 * Author: morgan
 *
 * Created on February 25, 2014, 6:40 PM
 */

#ifndef MO_MESH_H
#define	MO_MESH_H

#include <vector>
#include <initializer_list>
#include "vertex.hpp"
#include "range.hpp"

namespace mo {

/*!
 * A class that describes the geometric data to be rendered. Contains vertices
 * and elements, for vertex order, when rendering.
 */
class Mesh {
public:
    using Vertices = std::vector<Vertex>;
    using Elements = std::vector<int>;

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

    Mesh(std::initializer_list<Vertex> vertices, std::initializer_list<int> elements);
    Mesh();
    Mesh(const Mesh & mesh);
    virtual ~Mesh();	

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
    const Vertices & vertices();
    const Elements & elements();
    const Vertex * vertices_data() const;
    Vertices::size_type vertices_size() const;
    const int * elements_data() const;
    Elements::size_type elements_size() const;
private:
    static unsigned int current_id;
    unsigned int id_;
    Vertices vertices_;
    Elements elements_;
};

}

#endif	/* MO_MESH_H */

