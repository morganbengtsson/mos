#ifndef MOS_BATCH_HPP
#define MOS_BATCH_HPP

#include <vector>
#include <initializer_list>
#include <mos/graphics/model.hpp>
#include <mos/graphics/camera.hpp>
#include <mos/graphics/light.hpp>
#include <mos/graphics/fog_exp.hpp>
#include <mos/graphics/fog_linear.hpp>
#include <mos/graphics/target.hpp>

namespace mos {

class Batch {
public:
  /**
   * @brief The Shader enum
   */
  enum class Shader { TEXT, STANDARD, EFFECT, BLUR, CRT };

  /**
   * @brief How to draw the models.
   */
  enum class Draw { TRIANGLES, LINES, POINTS };

  Batch();

  template <class T>
  Batch(T begin, T end, const glm::mat4 &view, const glm::mat4 &projection,
        const glm::vec2 &resolution, const mos::Light &light = Light(),
        const mos::FogExp &fog_exp = FogExp(),
        const mos::FogLinear &fog_linear = FogLinear(),
        const Shader &shader = Shader::STANDARD,
        const Draw &draw = Draw::TRIANGLES)
      : models(begin, end), view(view), projection(projection), light(light),
        fog_exp(fog_exp), fog_linear(fog_linear), resolution(resolution), shader(shader), draw(draw) {}

  Batch(const std::initializer_list<mos::Model> &models, const glm::mat4 &view,
        const glm::mat4 &projection, const glm::vec2 &resolution,
        const mos::Light &light = Light(), const mos::FogExp &fog_exp = FogExp(),
        const mos::FogLinear &fog_linear = FogLinear(),
        const Shader &shader = Shader::STANDARD,
        const Draw &draw = Draw::TRIANGLES);
  std::vector<mos::Model> models;
  glm::mat4 view;
  glm::mat4 projection;
  mos::Light light;
  mos::FogExp fog_exp;
  mos::FogLinear fog_linear;
  glm::vec2 resolution;
  Shader shader;
  Draw draw;
};
}

#endif // MOS_BATCH_HPP
