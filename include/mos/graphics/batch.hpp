#ifndef MOS_BATCH_HPP
#define MOS_BATCH_HPP

#include <vector>
#include <initializer_list>
#include <mos/graphics/model.hpp>
#include <mos/graphics/camera.hpp>
#include <mos/graphics/light.hpp>
#include <mos/graphics/fog.hpp>

namespace mos {

class Batch {
public:
  enum class Shader { TEXT, STANDARD, EFFECT, BLUR, CRT};

  Batch();

  template <class T>
  Batch(T begin, T end, const glm::mat4 &view, const glm::mat4 &projection,
        const mos::Light &light, const mos::Fog &fog,
        const glm::vec2 &resolution, const Shader &shader = Shader::STANDARD)
      : models(begin, end), view(view), projection(projection), light(light),
        fog(fog), resolution(resolution), shader(shader) {}

  Batch(const std::initializer_list<mos::Model> &models, const glm::mat4 &view,
        const glm::mat4 &projection, const mos::Light &light,
        const mos::Fog &fog, const glm::vec2 &resolution,
        const Shader &shader = Shader::STANDARD);
  std::vector<mos::Model> models;
  glm::mat4 view;
  glm::mat4 projection;
  mos::Light light;
  mos::Fog fog;
  glm::vec2 resolution;
  Shader shader;
  // Shader
};
}

#endif // MOS_BATCH_HPP
