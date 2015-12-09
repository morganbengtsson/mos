#include "experimental/navmesh.hpp"
#include <glm/gtx/intersect.hpp>

namespace mos {

Navmesh::Navmesh(const Mesh & mesh) : Navmesh(mesh.vertices_begin(),
                                              mesh.vertices_end(),
                                              mesh.elements_begin(),
                                              mesh.elements_end()) {

}

bool Navmesh::intersects(const glm::vec3 & origin,
                         const glm::vec3 & direction) {
    for (auto & face : faces_){
        if (face.intersects(origin, direction)){
            return true;
        }
    }
    return false;
}

Navmesh::~Navmesh() {

}

Face::Face(const glm::vec3 & v0,
           const glm::vec3 & v1,
           const glm::vec3 & v2) :
    v0_(v0), v1_(v1), v2_(v2) {
}


bool Face::intersects(const glm::vec3 & origin,
                      const glm::vec3 & direction) {
    glm::vec3 barry;
    return glm::intersectRayTriangle(origin, direction, v0_, v1_, v2_, barry);
}

}
