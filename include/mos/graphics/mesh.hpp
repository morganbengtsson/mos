#ifndef MOS_MESH_H
#define MOS_MESH_H

#include <vector>
#include <initializer_list>
#include "vertex.hpp"

namespace mos {

/**
 * A class that describes the geometric data to be rendered. Contains vertices
 * and elements, for vertex order, when rendering.
 */
class Mesh {
  friend class Renderer;

public:
  /**
   * @brief Vertices container.
   */
  using Vertices = std::vector<Vertex>;

  /**
   * @brief Elements/indices container.
   */
  using Elements = std::vector<int>;

  template <class Tv, class Te>
  /**
   * @brief Mesh constructor.
   * @param vertices_begin Iterator to first vertex.
   * @param vertices_end Iterator to last vertex.
   * @param elements_begin Iterator to first element.
   * @param elements_end Iterator to last element.
   *
   * Mesh constructor that takes Vertices and elements (vertex order for
   *rendering).
   */
  Mesh(const Tv vertices_begin, const Tv vertices_end,
       Te elements_begin, Te elements_end)
      : valid_(false), vertices_(vertices_begin, vertices_end),
        elements_(elements_begin, elements_end) {
    id_ = current_id++;
  }

  /**
   * @brief Mesh constructor
   * @param vertices
   * @param elements
   */
  Mesh(std::initializer_list<Vertex> vertices,
       std::initializer_list<int> elements);

  /**
   * @brief Mesh
   */
  Mesh();

  /**
   * @brief Mesh copy constructor.
   * @param mesh
   */
  Mesh(const Mesh &mesh);

  /**
   * @brief ~Mesh destructor.
   */
  virtual ~Mesh();

  /**
   * @brief vertices_begin
   * @return Iterator to first Vertex.
   */
  Vertices::const_iterator vertices_begin() const;

  /**
   * @brief vertices_end
   * @return Iteartor to last vertex.
   */
  Vertices::const_iterator vertices_end() const;

  /**
   * @brief elements_begin
   * @return Iterator to first element.
   */
  Elements::const_iterator elements_begin() const;

  /**
   * @brief elements_end
   * @return Iterator to last element.
   */
  Elements::const_iterator elements_end() const;

  /**
   * @brief vertices_begin
   * @return Iterator to first vertex.
   */
  Vertices::iterator vertices_begin();

  /**
   * @brief vertices_end
   * @return Iterator to last vertex.
   */
  Vertices::iterator vertices_end();

  /**
   * @brief elements_begin
   * @return Iterator to first element.
   */
  Elements::iterator elements_begin();

  /**
   * @brief elements_end
   * @return Iterator to last element.
   */
  Elements::iterator elements_end();

  /**
   * @return A unique identifier.
   */
  unsigned int id() const;

  /**
   * @brief Tells the renderer if the mesh has been updated.
   */
  bool valid() const;

  /**
   * @brief invalidates the mesh, hence the data is updated.
   *
   */
  void invalidate();

  /**
   * @brief Clear the whole mesh.
   */
  void clear();

  /**
   * @brief Add a vertex.
   * @param vertex
   */
  void add(const Vertex vertex);

  /**
   * @brief Add an element
   * @param element
   */
  void add(const int element);

  /**
   * @brief vertices
   * @return const reference of Vertices.
   */
  const Vertices &vertices();

  /**
   * @brief elements
   * @return const reference of Elements.
   */
  const Elements &elements();

  /**
   * @brief vertices_data
   * @return cosnt Vertex array.
   */
  const Vertex *vertices_data() const;

  /**
   * @brief vertices_size
   * @return Number of vertices.
   */
  Vertices::size_type vertices_size() const;

  /**
   * @brief elements_data
   * @return const int array.
   */
  const int *elements_data() const;

  /**
   * @brief elements_size
   * @return Number of elements.
   */
  Elements::size_type elements_size() const;

  /**
   * @brief mix
   * @param mesh1
   * @param mesh2
   * @param amount
   */
  void mix(const Mesh &mesh1, const Mesh &mesh2, const float amount);

private:
  static unsigned int current_id;
  unsigned int id_;
  Vertices vertices_;
  Elements elements_;
  bool valid_;
};
}

#endif /* MOS_MESH_H */
