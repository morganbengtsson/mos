#include <mos/render/render_cube_camera.hpp>
#include <glm/gtc/matrix_transform.hpp>
namespace mos {

RenderCubeCamera::RenderCubeCamera(const glm::vec3 &position,
                                   const glm::vec2 &resolution) : projection(glm::perspective(glm::half_pi<float>(),
                                                                                              resolution.x / resolution.y,
                                                                                              0.01f,
                                                                                              100.0f)),
                                                                  up(glm::vec3(0.0f, 0.0f, 1.0f)),
    cameras{{mos::RenderCamera(position, position + glm::vec3(1.0f, 0.0f, 0.0f), projection, glm::vec3(0.0f, 1.0f, 0.0f), resolution),
             mos::RenderCamera(position, position + glm::vec3(-1.0f, 0.0f, 0.0f), projection, glm::vec3(0.0f, 1.0f, 0.0f), resolution),
             mos::RenderCamera(position, position + glm::vec3(0.0f, 1.0f, 0.0f), projection, glm::vec3(0.0f, 0.0f, -1.0f), resolution),
             mos::RenderCamera(position, position + glm::vec3(0.0f, -1.0f, 0.0f), projection, glm::vec3(0.0f, 0.0f, 1.0f), resolution),
             mos::RenderCamera(position, position + glm::vec3(0.0f, 0.0f, 1.0f), projection, glm::vec3(0.0f, 1.0f, 0.0f), resolution),
             mos::RenderCamera(position, position + glm::vec3(0.0f, 0.0f, -1.0f), projection, glm::vec3(0.0f, 1.0f, 0.0f), resolution)}}
    {}
}