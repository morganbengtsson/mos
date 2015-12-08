#ifndef MOS_NAVMESH_HPP
#define MOS_NAVMESH_HPP

#include <vector>
#include <array>
#include <glm/glm.hpp>
#include <algorithm>
#include "../graphics/mesh.hpp"
#include "../graphics/box.hpp"

namespace mos {


class Face{
public:
    Face(const glm::vec3 & v0,
         const glm::vec3 & v1,
         const glm::vec3 & v2);
    bool intersects(const Box & box);
private:
    const glm::vec3 & v0_, v1_, v2_;
};

class Navmesh
{
public:
    Navmesh(const Mesh & mesh);
    template<class Tv, class Te>
    Navmesh(Tv vertices_begin,
            Tv vertices_end,
            Te elements_begin,
            Te elements_end) :
        elements_(elements_begin, elements_end) {
        for (auto it = vertices_begin; it != vertices_end; it++){
            vertices_.push_back(it->position);
        }

        if (elements_.empty()) {
            for (auto it = vertices_.begin(); it != vertices_.end();) {
                auto & v1 = *it;
                it++;
                auto & v2 = *it;
                it++;
                auto & v3 = *it;
                it++;
                faces_.push_back(Face(v1, v2, v3));
            }

        } else {
            for (auto it = elements_.begin(); it != elements_.end();) {
                auto & v1 = vertices_[*it];
                it++;
                auto & v2 = vertices_[*it];
                it++;
                auto & v3 = vertices_[*it];
                it++;
                faces_.push_back(Face(v1, v2, v3));
            }
        }
    }
    bool intersects(const Box & box);

    ~Navmesh();
private:
    std::vector<Face> faces_;
    std::vector<glm::vec3> vertices_;
    std::vector<int> elements_;

};

}

#endif // MOS_NAVMESH_HPP
