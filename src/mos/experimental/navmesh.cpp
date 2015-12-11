#include "experimental/navmesh.hpp"
#include <glm/gtx/intersect.hpp>

namespace mos {

Navmesh::Navmesh(const Mesh & mesh) : Navmesh(mesh.vertices_begin(),
                                              mesh.vertices_end(),
                                              mesh.elements_begin(),
                                              mesh.elements_end()) {

}

std::experimental::optional<glm::vec3> Navmesh::intersects(const glm::vec3 & origin,
               const glm::vec3 & direction){
    for (auto & face : faces_){
        auto intersection = face.intersects(origin, direction);
        if (intersection){
            return intersection;
        }
    }
    return std::experimental::optional<glm::vec3>();
}

Navmesh::~Navmesh() {

}

Face::Face(const glm::vec3 & v0,
           const glm::vec3 & v1,
           const glm::vec3 & v2) :
    v0_(v0), v1_(v1), v2_(v2) {
}


std::experimental::optional<glm::vec3> Face::intersects(const glm::vec3 & origin,
                      const glm::vec3 & direction) {
    glm::vec3 bary;
    auto intersects = glm::intersectRayTriangle(origin, direction, v0_, v1_, v2_, bary);

    if (intersects) {
        return std::experimental::optional<glm::vec3>(origin + direction * bary.z);
    }
    else {
        return std::experimental::optional<glm::vec3>();
    }
}

}
