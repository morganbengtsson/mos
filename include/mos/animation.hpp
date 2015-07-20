/**
 * @file assets.hpp
 * @author: Morgan Bengtsson <bengtsson.morgan@gmail.com>
 */

#ifndef MOS_ANIMATION_HPP
#define MOS_ANIMATION_HPP

#include <map>
#include <memory>
#include <utility>
#include <initializer_list>
#include "mesh.hpp"

namespace mos {

/**
 * @brief The Animation class
 * Describes an animation in form of keys and Meshes. Interpolates between them.
 */
class Animation
{
public:
    /**
     * @brief Animation constructor.
     * @param keyframes Map of keys and shared Meshes
     */
    Animation(std::initializer_list<std::pair<unsigned int, std::shared_ptr<Mesh const>>> keyframes);

    /**
     *@brief Destructor.
     */
    ~Animation();

    /**
     * @brief update, move the animation forward.
     * @param dt How much time to progress.
     */
    void update(const float dt);

    /**
     * @brief frame
     * @return
     */
    int frame() const;

    /**
     * @brief reset, restart the animation.
     */
    void reset();

    /**
     * @brief The mesh, which is animated.
     * @return shared_ptr to mesh.
     */
    std::shared_ptr<Mesh> mesh();

    /**
     * @brief Special transition mesh.
     *
     * If this mesh is present, the animation system uses it as the first key.
     * An experimental way to provide smooth transitions
     */
    std::shared_ptr<Mesh> transition;

private:
    int transition_index_;
    float time_;
    std::map<unsigned int, std::shared_ptr<const Mesh>> keyframes_;
    std::shared_ptr<Mesh> mesh_;
    int frames_per_second_;
};
}

#endif // MOS_ANIMATION_HPP
