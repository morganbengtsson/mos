#ifndef MOS_BATCH_HPP
#define MOS_BATCH_HPP

#include <vector>
#include <initializer_list>
#include <mos/render/model.hpp>
#include <mos/render/render_camera.hpp>
#include <mos/render/light.hpp>
#include <mos/render/fog_exp.hpp>
#include <mos/render/fog_linear.hpp>
#include <mos/render/render_target.hpp>
#include <mos/render/texture_cube.hpp>

namespace mos {

class ModelsBatch {
public:
  /**
   * @brief The Shader enum
   */
  enum class Shader { TEXT, STANDARD, EFFECT, BLUR, CRT };

  /**
   * @brief How to draw the models.
   */
  enum class Draw { TRIANGLES, LINES, POINTS };

  ModelsBatch();

  template <class T>
  ModelsBatch(T begin, T end, const Camera &camera, const Light &light = Light(),
        const mos::FogExp &fog_exp = FogExp(),
        const mos::FogLinear &fog_linear = FogLinear(),
        const Shader &shader = Shader::STANDARD,
        const Draw &draw = Draw::TRIANGLES)
      : models(begin, end), camera(camera), light(light),
        fog_exp(fog_exp), fog_linear(fog_linear), shader(shader), draw(draw) {}

  ModelsBatch(const std::initializer_list<mos::Model> &models, const Camera &camera,
        const mos::Light &light = Light(), const mos::FogExp &fog_exp = FogExp(),
        const mos::FogLinear &fog_linear = FogLinear(),
        const Shader &shader = Shader::STANDARD,
        const Draw &draw = Draw::TRIANGLES);
  std::vector<mos::Model> models;
  Light light;
  Camera camera;
  FogExp fog_exp;
  FogLinear fog_linear;
  Shader shader;
  Draw draw;
};
}

#endif // MOS_BATCH_HPP
