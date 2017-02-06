#ifndef MOS_RENDERCUBECAMERA_HPP
#define MOS_RENDERCUBECAMERA_HPP

#include <glm/glm.hpp>
#include <array>


namespace mos {
class RenderCubeCamera {
public:
  RenderCubeCamera(const glm::vec3 &position = glm::vec3(0.0f, 0.0f, 2.0f),
                   const glm::vec2 &resolution = glm::vec2(128.0f, 128.0f));

  glm::vec3 position() const;
  void position(const glm::vec3 &position);
  float aspect_ratio() const;
  std::array<glm::mat4, 6> views;
  const glm::vec2 resolution;
  const glm::mat4 projection;


private:
  void update_views();
  glm::vec3 position_;
};
}

#endif //MOS_RENDERCUBECAMERA_HPP
