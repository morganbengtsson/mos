#ifndef MOS_SOUNDSOURCE_HPP
#define MOS_SOUNDSOURCE_HPP

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "sound.hpp"
#include "source.hpp"

namespace mos {

/**
 * A classed used for audio playback. Contains sounds, together with a position.
 * That is used if 3D audio is enabled.
 */
class SoundSource {
public:
  /**
   * @brief SoundSource constructor.
   * @param sound
   * @param source
   */
  explicit SoundSource(const std::shared_ptr<Sound> &sound,
                       const Source &source = Source());
  virtual ~SoundSource();

  /**
   * @brief update the source and stop if not looping.
   * @param dt
   */
  void update(const float dt);

  /**
   * @brief The sound that is played from the source.
   */
  std::shared_ptr<Sound> sound;
  Source source;
private:
  float time_;
};
}

#endif // MOS_SOUNDSOURCE_HPP
