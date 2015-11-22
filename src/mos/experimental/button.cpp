#include <experimental/button.hpp>

namespace mos {

Button::Button(const Text & text) :text_(text) {
    mos::Vertex v1(-10.5f, -10.5f, 0.0f);
    mos::Vertex v2(-10.5f, 10.5f, 0.0f);
    mos::Vertex v3(10.5f, 10.5f, 0.0f);
    mos::Vertex v4(10.5f, -10.5f, 0.0f);

    mos::Mesh mesh({v1, v2, v3, v4}, {0, 2, 1, 0, 3, 2});
    rectangle_.mesh = std::make_shared<Mesh>(mesh);
    rectangle_.mesh->invalidate();
    rectangle_.transform(glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, 10.0f)));
    mos::Material material(glm::vec3(1.0f), glm::vec3(1.0f));
    material.opacity = 0.5f;
    rectangle_.opacity = 1.0f;
    rectangle_.material = std::make_shared<Material>(material);
    rectangle_.receives_light = false;
    rectangle_.shader = Model::Shader::TEXT;
}

Button::~Button() {
}

Model Button::model() {
    return rectangle_;
}

}
