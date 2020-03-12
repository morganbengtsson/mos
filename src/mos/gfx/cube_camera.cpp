#include <mos/gfx/cube_camera.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace mos {
namespace gfx {

Cube_camera::Cube_camera(const glm::vec3 &position, const float near, const float far) :
    cameras{{Camera(position,
                    position + glm::vec3(1.0f,
                                         0.0f,
                                         0.0f),
                    glm::perspective(glm::half_pi<float>(),
                                     1.0f,
                                     near,
                                     far),
                    glm::vec3(0.0f, -1.0f, 0.0f)),
             Camera(position,
                    position + glm::vec3(-1.0f,
                                         0.0f,
                                         0.0f),
                    cameras[0].projection(),
                    glm::vec3(0.0f, -1.0f, 0.0f)),
             Camera(position,
                    position + glm::vec3(0.0f,
                                         1.0f,
                                         0.0f),
                    cameras[0].projection(),
                    glm::vec3(0.0f, 0.0f, 1.0f)),
             Camera(position,
                    position + glm::vec3(0.0f,
                                         -1.0f,
                                         0.0f),
                    cameras[0].projection(),
                    glm::vec3(0.0f, 0.0f, -1.0f)),
             Camera(position,
                    position + glm::vec3(0.0f,
                                         0.0f,
                                         1.0f),
                    cameras[0].projection(),
                    glm::vec3(0.0f, -1.0f, 0.0f)),
             Camera(position,
                    position + glm::vec3(0.0f,
                                         0.0f,
                                         -1.0f),
                    cameras[0].projection(),
                    glm::vec3(0.0f,
                              -1.0f,
                              0.0f))}} {}
glm::vec3 Cube_camera::position() const {
  return cameras[0].position();
}

void Cube_camera::position(const glm::vec3 &position) {
  cameras[0].position(position);
  cameras[0].direction(glm::vec3(1.0f, 0.0f, 0.0f));
  cameras[1].position(position);
  cameras[1].direction(glm::vec3(-1.0f, 0.0f, 0.0f));
  cameras[2].position(position);
  cameras[2].direction(glm::vec3(0.0f, 1.0f, 0.0f));
  cameras[3].position(position);
  cameras[3].direction(glm::vec3(0.0f, -1.0f, 0.0f));
  cameras[4].position(position);
  cameras[4].direction(glm::vec3(0.0f, 0.0f, 1.0f));
  cameras[5].position(position);
  cameras[5].direction(glm::vec3(0.0f, 0.0f, -1.0f));
}

void Cube_camera::near_far(const float near, const float far) {
  for (auto &camera : cameras) {
    camera.projection(glm::perspective(glm::half_pi<float>(),
                                         1.0f,
                                         near,
                                         far));
  }
}
}
}
