#ifndef MO_ANIMATION_HPP
#define MO_ANIMATION_HPP

#include <map>
#include <memory>
#include <utility>
#include <initializer_list>
#include "mesh.hpp"

namespace mo {

class Animation
{
public:
    Animation(std::initializer_list<std::pair<unsigned int,std::shared_ptr<Mesh>>> keyframes);
    ~Animation();
    void update(const float dt);
    std::shared_ptr<Mesh> mesh();
private:
    float time_;
    std::map<unsigned int, std::shared_ptr<Mesh>> keyframes_;
    std::shared_ptr<Mesh> mesh_;
    int frames_per_second_;
};
}

#endif // MO_ANIMATION_HPP
