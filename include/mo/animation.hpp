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
    Animation(std::initializer_list<std::pair<unsigned int, std::shared_ptr<Mesh const>>> keyframes);
    ~Animation();
    void update(const float dt);
    int frame() const;
    void reset();
    std::shared_ptr<Mesh> mesh();
    std::shared_ptr<Mesh> transition;

private:
    int transition_index_;
    float time_;
    std::map<unsigned int, std::shared_ptr<const Mesh>> keyframes_;
    std::shared_ptr<Mesh> mesh_;
    int frames_per_second_;
};
}

#endif // MO_ANIMATION_HPP
