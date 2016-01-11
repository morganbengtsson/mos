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
class Animation {
public:
  /**
   * @brief Animation default contructor
   */
  Animation(const unsigned int frames_per_second = 30);
  /**
   * @brief Animation constructor.
   * @param keyframes Map of keys and shared Meshes
   */
  Animation(std::initializer_list<
      std::pair<unsigned int, std::shared_ptr<Mesh const>>> keyframes);

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
  unsigned int frame() const;

  /**
   * @brief Add a keyframe
   * @param key
   * @param mesh shared_ptr
   */
  void keyframe(const unsigned int key, const std::shared_ptr<Mesh> mesh);

  /**
   * @brief reset, restart the animation.
   */
  void reset();

  /**
   * @brief frames_per_second
   */
  void frames_per_second(const unsigned int frames_per_second);

  /**
   * @brief frames_per_second
   * @return
   */
  unsigned int frames_per_second() const;

  /**
   * @brief The mesh, which is animated.
   * @return shared_ptr to mesh.
   */
  std::shared_ptr<Mesh> mesh();

private:
  float time_;
  std::map<unsigned int, std::shared_ptr<const Mesh>> keyframes_;
  std::shared_ptr<Mesh> mesh_;
  unsigned int frames_per_second_;
};
}

#endif // MOS_ANIMATION_HPP
