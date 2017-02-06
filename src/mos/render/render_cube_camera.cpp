#include <mos/render/render_cube_camera.hpp>
#include <glm/gtc/matrix_transform.hpp>
namespace mos {

RenderCubeCamera::RenderCubeCamera(const glm::vec3 &position,
                                   const glm::vec2 &resolution) : projection(glm::perspective(glm::pi<float>() / 2.0f,
                                                                                              resolution.x / resolution.y,
                                                                                              0.1f,
                                                                                              100.0f)),
                                                                  up(glm::vec3(0.0f, 0.0f, 1.0f)),
    cameras{{mos::RenderCamera(position, position + glm::vec3(1.0f, 0.0f, 0.0f), projection, up, resolution),
             mos::RenderCamera(position, position + glm::vec3(-11.0f, 0.0f, 0.0f), projection, up, resolution),
             mos::RenderCamera(position, position + glm::vec3(0.0f, 1.0f, 0.0f), projection, up, resolution),
             mos::RenderCamera(position, position + glm::vec3(0.0f, -1.0f, 0.0f), projection, up, resolution),
             mos::RenderCamera(position, position + glm::vec3(0.0f, 0.0f, 1.0f), projection, up, resolution),
             mos::RenderCamera(position, position + glm::vec3(0.0f, 0.0f, -1.0f), projection, up, resolution)}}
    {}

}