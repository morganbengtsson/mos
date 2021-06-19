#include <mos/gfx/cube_camera.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace mos::gfx {

namespace  {

const glm::vec3 x_axis{1.0f, 0.0f, 1.0f};
const glm::vec3 y_axis{0.0f, 1.0f, 0.0f};
const glm::vec3 z_axis{0.0f, 0.0f, 1.0f};

} // namespace

Cube_camera::Cube_camera(const glm::vec3 &position, const float near,
                         const float far)
    : cameras{
          {Camera(position, position + x_axis,
                  glm::perspective(glm::half_pi<float>(), 1.0f, near, far),
                  -y_axis),
           Camera(position, position - x_axis, cameras[0].projection(),
                  -y_axis),
           Camera(position, position + y_axis, cameras[0].projection(), z_axis),
           Camera(position, position - y_axis, cameras[0].projection(),
                  -z_axis),
           Camera(position, position + z_axis, cameras[0].projection(),
                  -y_axis),
           Camera(position, position - z_axis, cameras[0].projection(),
                  -y_axis)}} {}
auto Cube_camera::position() const -> glm::vec3 {
  return cameras[0].position();
}

void Cube_camera::position(const glm::vec3 &position) {
  cameras[0].position(position);
  cameras[0].direction(x_axis);
  cameras[1].position(position);
  cameras[1].direction(-x_axis);
  cameras[2].position(position);
  cameras[2].direction(y_axis);
  cameras[3].position(position);
  cameras[3].direction(-y_axis);
  cameras[4].position(position);
  cameras[4].direction(z_axis);
  cameras[5].position(position);
  cameras[5].direction(-z_axis);
}

void Cube_camera::near_far(const float near, const float far) {
  for (auto &camera : cameras) {
    camera.projection(glm::perspective(glm::half_pi<float>(), 1.0f, near, far));
  }
}
} // namespace mos::gfx
