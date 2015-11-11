/* 
 * File:   Model.cpp
 * Author: morgan
 *
 * Created on March 5, 2014, 10:29 PM
 */

#include "graphics/model.hpp"

namespace mos {

Model::Model(){

}
Model::Model(const std::shared_ptr<Mesh> & mesh,
             const std::shared_ptr<Texture2D> & texture,
             const std::shared_ptr<Texture2D> & texture2,
             const glm::mat4 & transform,
             const glm::mat4 & parent_transform,
             const Draw draw,
             const std::shared_ptr<Material> &  material,
             const std::shared_ptr<Texture2D> & lightmap,
             const std::shared_ptr<Texture2D> & normalmap,
             const bool selectable,
             const bool step,
             const float obstruction,
             const float opacity,
             const float affected_by_light) :
    draw(draw),
    mesh(mesh),
    texture(texture),
    texture2(texture2),
    material(material),
    lightmap(lightmap),
    normalmap(normalmap),
    transform_(transform),
    valid_(true),
    selected_(false),
    selectable(selectable),
    box(mesh->vertices_begin(), mesh->vertices_end(), transform * parent_transform, obstruction),
    opacity(opacity),
    receives_light(affected_by_light) {
        box.step(step);
}

Model::~Model() {
}

glm::mat4 Model::transform() const{
    return transform_;
}

void Model::transform_box(const glm::mat4 & transform){
    box.transform(transform);
    for (auto & model : models){
        model.transform_box(transform * model.transform());
    }
}

void Model::transform(const glm::mat4 & transform,
                      const glm::mat4 & parent_transform) {
    transform_ = transform;
    transform_box(transform * parent_transform);
}

glm::vec4 Model::color() const {
    return glm::vec4(material->diffuse.r, material->diffuse.g, material->diffuse.b, material->opacity);
}

glm::vec3 Model::position() const {
    return (glm::vec3)(transform()*glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
}

void Model::position(const glm::vec3 position){
    auto t = transform();
    t[3][0] = position[0];
    t[3][1] = position[1];
    t[3][2] = position[2];
    transform(t);
}

bool Model::selected() const {
    return selected_;
}

void Model::selected(const bool selected) {
    selected_ = selectable ? selected : false;
}

BoxIntersection Model::intersects(const Model &model) const {

    // Non working recursive version.
    /*
    auto intersection = box.intersects(model.box);
    if (!intersection.intersects){
        for (auto & child_model : models){
            auto child_intersection = child_model.intersects(model);
            if (child_intersection.intersects){
                return child_intersection;
            } else {
                return child_model.intersects(model);
            }
        }
    }
    return intersection;
    */

    //TODO: Make recursive.
    auto parent_intersection = box.intersects(model.box);
    if (!parent_intersection.intersects){
        for (auto & child : models){
            auto child_intersection = child.box.intersects(model.box);
            if (child_intersection.intersects){
                return child_intersection;
            }
            else {
                for (auto & child2 : child.models) {
                    auto child2_intersection = child2.box.intersects(model.box);
                    if (child2_intersection.intersects) {
                        return child2_intersection;
                    }
                    else {
                        for (auto & child3 : child2.models) {
                            auto child3_intersection = child3.box.intersects(model.box);
                            if (child3_intersection.intersects) {
                                return child3_intersection;
                            }
                        }
                    }
                }
            }
        }
    }
    return parent_intersection;
}

}
