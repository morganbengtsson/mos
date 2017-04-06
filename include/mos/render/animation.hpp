#ifndef MOS_ANIMATION_HPP
#define MOS_ANIMATION_HPP

#include <initializer_list>
#include <map>
#include <memory>
#include <mos/render/mesh.hpp>
#include <utility>

namespace mos {

/**
 * @brief The Animation class
 * Describes an animation in form of keys and Meshes. Interpolates between them.
 */
class Animation {
public:
  Animation();
  /**
   * @brief Animation default contructor
   */
  Animation(const std::map<unsigned int, std::shared_ptr<Mesh const>> keyframes,
            const unsigned int frame_rate = 30);
  /**
   * @brief Animation constructor.
   * @param keyframes Map of keys and shared Meshes
   */
  Animation(std::initializer_list<
                std::pair<unsigned int, std::shared_ptr<Mesh const>>>
                keyframes,
            const unsigned int frame_rate = 30);

  /**
   *@brief Destructor.
   */
  ~Animation();

  Animation(const std::string &path);

  /**
   * @brief update, move the animation forward.
   * @param dt How much time to progress.
   */
  void update(const float dt);

  /**
   * @brief Current frame
   * @return frame
   */
  unsigned int frame() const;

  /**
   * @brief reset, restart the animation.
   */
  void reset();

  /**
   * @brief Set frame rate.
   */
  void frame_rate(const unsigned int frame_rate);

  /**
   * @brief Frame rate in frames per second.
   * @return
   */
  unsigned int frame_rate() const;

  /**
   * @brief The mesh, which is animated.
   * @return shared_ptr to mesh.
   */
  std::shared_ptr<Mesh> mesh();

private:
  float time_;
  std::map<unsigned int, std::shared_ptr<const Mesh>> keyframes_;
  std::shared_ptr<Mesh> mesh_;
  unsigned int frame_rate_;
};
}

#endif // MOS_ANIMATION_HPP
