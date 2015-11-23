#include <experimental/button.hpp>

namespace mos {

Button::Button(const Text & text) :text_(text) {
    mos::Vertex v1(-0.5f, -0.5f, 0.0f);
    mos::Vertex v2(-0.5f, 0.5f, 0.0f);
    mos::Vertex v3(0.5f, 0.5f, 0.0f);
    mos::Vertex v4(0.5f, -0.5f, 0.0f);

    mos::Mesh mesh({v1, v2, v3, v4}, {0, 2, 1, 0, 3, 2});
    rectangle_.mesh = std::make_shared<Mesh>(mesh);
    rectangle_.transform(glm::scale(glm::mat4(1.0f), glm::vec3(text_.width()*2.0f, 30.0f, 1.0f)));
    mos::Material material(glm::vec3(1.0f), glm::vec3(0.5f, 0.5f, 1.0f));
    material.opacity = 0.5f;
    rectangle_.material = std::make_shared<Material>(material);
    rectangle_.receives_light = false;
    rectangle_.shader = Model::Shader::STANDARD;
     rectangle_.mesh->invalidate();
}

Button::~Button() {
}

Model Button::model() {
    mos::Model out;
    //out.models.push_back(rectangle_);
    out.models.push_back(text_.model());
    return out;
}

}
