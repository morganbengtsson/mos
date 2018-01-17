#ifndef MOS_NAVMESH2_HPP
#define MOS_NAVMESH2_HPP

#include <vector>
#include <array>
#include <glm/glm.hpp>
#include <algorithm>
#include <experimental/optional>
#include <iostream>
#include <glm/gtx/io.hpp>
#include <mos/simulate/intersection.hpp>
#include <mos/render/mesh.hpp>

namespace mos {

class Face2 {
public:
  Face2(Vertex &v0, Vertex &v1, Vertex &v2);
  std::experimental::optional<Vertex>
  intersects(const glm::vec3 &origin, const glm::vec3 &direction);
public :
  Vertex v0_;
  Vertex v1_;
  Vertex v2_;
};

class Navmesh2 {
public:
  using OptionalIntersection = std::experimental::optional<Vertex>;
  Navmesh2();
  Navmesh2(const Mesh &mesh, const glm::mat4 &transform);

  template <class Tv, class Te>
  Navmesh2(Tv vertices_begin, Tv vertices_end, Te elements_begin,
          Te elements_end, const glm::mat4 &transform)
      : elements_(elements_begin, elements_end), vertices_(vertices_begin, vertices_end){

    for(auto & vertex : vertices_) {
      vertex.position = glm::vec3(transform *
          glm::vec4(vertex.position, 1.0f));
    }

    /*
    if (elements_.empty()) {
      for (int i = 0; i < vertices_.size(); i+= 3){
        faces_.push_back(Face2(vertices_[i], vertices_[i + 1], vertices_[i + 2]));
      }
    } else {
      for (int i = 0; i < elements_.size(); i += 3){
        faces_.push_back(Face2(vertices_[elements_[i]],
                               vertices_[elements_[i + 1]],
                               vertices_[elements_[i +2]]));
      }
    }*/
  }
  std::experimental::optional<Vertex>
  intersects(const glm::vec3 &origin, const glm::vec3 &direction);

  std::experimental::optional<Vertex>
  closest_intersection(const glm::vec3 &origin, const glm::vec3 &direction);

  ~Navmesh2();

public:
  //std::vector<Face2> faces_;
  std::vector<Vertex> vertices_;
  std::vector<int> elements_;
};
}

#endif // MOS_NAVMESH2_HPP
