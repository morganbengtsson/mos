#ifndef MOS_MESH_H
#define MOS_MESH_H

#include <vector>
#include <initializer_list>
#include <string>
#include <atomic>
#include <memory>
#include <mos/render/vertex.hpp>

namespace mos {
  class Mesh;
  using SharedMesh = std::shared_ptr<Mesh>;
/**
 * A class that describes the geometric data to be rendered. Contains vertices
 * and elements, for vertex order, when rendering.
 */
class Mesh {
  friend class RenderSystem;

public:
  using Positions = std::vector<glm::vec3>;
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
        elements_(elements_begin, elements_end), id_(current_id++) {}

  /**
   * @brief Mesh constructor
   * @param vertices
   * @param elements
   */
  Mesh(std::initializer_list<Vertex> vertices,
       std::initializer_list<int> elements);

  /**
   * @brief Mesh constructor from file.
   * @param path
   */
  Mesh(const std::string &path);

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
  ~Mesh();

  static SharedMesh load(const std::string &path);

  Vertices::iterator begin();
  Vertices::iterator end();

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
  void add(const Vertex& vertex);

  template<class T>
  void append(T begin, T end){
    vertices_.insert(vertices_.end(), begin, end);
    invalidate();
  }

  /**
   * @brief Add an element
   * @param element
   */
  void add(const int element);

  void pop_front(const int num) {
    vertices_.erase(vertices_.begin(), vertices_.begin() + num);
    valid_ = false;
  }

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
   * @brief Get a copy of positions.
   * @return
   */
  Positions positions() const;
  /**
   * @brief mix
   * @param mesh1
   * @param mesh2
   * @param amount
   */
  void mix(const Mesh &mesh1, const Mesh &mesh2, const float amount);

  void apply_transform(const glm::mat4 &transform);

  void calculate_normals();

private:
  void calculate_tangents(mos::Vertex &v0, mos::Vertex &v1, mos::Vertex &v2);
  static std::atomic_uint current_id;
  unsigned int id_;
  Vertices vertices_;
  Elements elements_;
  bool valid_;
};
}

#endif /* MOS_MESH_H */
