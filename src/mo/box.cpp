#include "box.hpp"

#include <utility>
#include <stdexcept>
#include <iostream>
#include <cassert>
#include <array>
#include <glm/gtx/io.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/component_wise.hpp>

namespace mo {

Box::Box(){
}

Box::Box(const glm::vec3 & min,
         const glm::vec3 & max,
         const glm::mat4 & transform,
         const float obstruction) :
    transform_(transform),
    min_(min),
    max_(max),
    obstruction_(obstruction) {
        if(glm::all(glm::lessThan(max_, min_))){
            throw std::invalid_argument("Min must be less than max.");
        }
}

glm::vec3 Box::min() const {
    return (glm::vec3)(transform() * glm::vec4(min_, 1.0f));
}
glm::vec3 Box::max() const {
    return (glm::vec3)(transform() * glm::vec4(max_, 1.0f));
}

RayIntersection Box::intersect(const glm::vec3 & origin, const glm::vec3 direction, float t1, float t2) {
    // Intersection method from Real-Time Rendering and Essential Mathematics for Games
    glm::mat4 model_matrix = transform();
    glm::vec3 ray_origin = origin;
    glm::vec3 ray_direction = direction;
    glm::vec3 aabb_min = min_;
    glm::vec3 aabb_max = max_;

    float t_min = 0.0f;
    float t_max = 100000.0f;
    const float limit = 0.001f;

    //glm::vec3 position_worldspace(model_matrix[3].x, model_matrix[3].y, model_matrix[3].z);
    glm::vec3 position_worldspace = (glm::vec3)(transform()*glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    glm::vec3 delta = position_worldspace - ray_origin;

    // Test intersection with the 2 planes perpendicular to the OBB's X axis
    {
        glm::vec3 xaxis(model_matrix[0].x, model_matrix[0].y, model_matrix[0].z);
        float e = glm::dot(xaxis, delta);
        float f = glm::dot(ray_direction, xaxis);

        if ( fabs(f) > limit ){ // Standard case

            float t1 = (e+aabb_min.x)/f; // Intersection with the "left" plane
            float t2 = (e+aabb_max.x)/f; // Intersection with the "right" plane
            // t1 and t2 now contain distances betwen ray origin and ray-plane intersections

            // We want t1 to represent the nearest intersection,
            // so if it's not the case, invert t1 and t2
            if (t1>t2){
                float w=t1;t1=t2;t2=w; // swap t1 and t2
            }

            // tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
            if ( t2 < t_max ){
                t_max = t2;
            }
            // tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
            if ( t1 > t_min ){
                t_min = t1;
            }

            // And here's the trick :
            // If "far" is closer than "near", then there is NO intersection.
            // See the images in the tutorials for the visual explanation.
            if (t_max < t_min ){
                return RayIntersection(false, 0.0f);
            }

        }else{ // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
            if(-e+aabb_min.x > 0.0f || -e+aabb_max.x < 0.0f){
                return RayIntersection(false, 0.0f);
            }
        }
    }


    // Test intersection with the 2 planes perpendicular to the OBB's Y axis
    // Exactly the same thing than above.
    {
        glm::vec3 yaxis(model_matrix[1].x, model_matrix[1].y, model_matrix[1].z);
        float e = glm::dot(yaxis, delta);
        float f = glm::dot(ray_direction, yaxis);

        if ( fabs(f) > limit ){

            float t1 = (e+aabb_min.y)/f;
            float t2 = (e+aabb_max.y)/f;

            if (t1>t2){
                float w=t1;t1=t2;t2=w;
            }

            if ( t2 < t_max ){
                t_max = t2;
            }
            if ( t1 > t_min ){
                t_min = t1;
            }
            if (t_min > t_max){
                return RayIntersection(false, 0.0f);
            }

        }else{
            if(-e+aabb_min.y > 0.0f || -e+aabb_max.y < 0.0f){
                return RayIntersection(false, 0.0f);
            }
        }
    }


    // Test intersection with the 2 planes perpendicular to the OBB's Z axis
    // Exactly the same thing than above.
    {
        glm::vec3 zaxis(model_matrix[2].x, model_matrix[2].y, model_matrix[2].z);
        float e = glm::dot(zaxis, delta);
        float f = glm::dot(ray_direction, zaxis);

        if ( fabs(f) > limit ){

            float t1 = (e+aabb_min.z)/f;
            float t2 = (e+aabb_max.z)/f;

            if (t1>t2){
                float w=t1;t1=t2;t2=w;
            }

            if ( t2 < t_max ){
                t_max = t2;
            }
            if ( t1 > t_min ){
                t_min = t1;
            }
            if (t_min > t_max){
                return RayIntersection(false, 0.0f);
            }

        }else{
            if(-e+aabb_min.z > 0.0f || -e+aabb_max.z < 0.0f)
                return RayIntersection(false, 0.0f);
        }
    }

    float intersection_distance = t_min;
    return RayIntersection(true, intersection_distance);
}

RayIntersection Box::intersect(glm::vec3 point1, glm::vec3 point2) {
    return intersect(point1, glm::normalize(point2 - point1), 0.0f, glm::distance(point1, point2));
}

BoxIntersection Box::intersects(const Box &other) {
    static const std::array<glm::vec3, 6> faces = {
        glm::vec3(-1, 0, 0), // 'left' face normal (-x direction)
        glm::vec3( 1, 0, 0), // 'right' face normal (+x direction)
        glm::vec3( 0,-1, 0), // 'bottom' face normal (-y direction)
        glm::vec3( 0, 1, 0), // 'top' face normal (+y direction)
        glm::vec3( 0, 0,-1), // 'far' face normal (-z direction)
        glm::vec3( 0, 0, 1), // 'near' face normal (+x direction)
    };

    glm::vec3 maxa = this->max();      
    glm::vec3 mina = this->min();

    if (maxa.x < mina.x){std::swap(maxa.x, mina.x);}
    if (maxa.y < mina.y){std::swap(maxa.y, mina.y);}
    if (maxa.z < mina.z){std::swap(maxa.z, mina.z);}

    glm::vec3 maxb = other.max();
    glm::vec3 minb = other.min();


    if (maxb.x < minb.x){std::swap(maxb.x, minb.x);}
    if (maxb.y < minb.y){std::swap(maxb.y, minb.y);}
    if (maxb.z < minb.z){std::swap(maxb.z, minb.z);}

    std::array<float, 6> distances = {
        (maxb.x - mina.x), // distance of box 'b' to face on 'left' side of 'a'.
        (maxa.x - minb.x), // distance of box 'b' to face on 'right' side of 'a'.
        (maxb.y - mina.y), // distance of box 'b' to face on 'bottom' side of 'a'.
        (maxa.y - minb.y), // distance of box 'b' to face on 'top' side of 'a'.
        (maxb.z - mina.z), // distance of box 'b' to face on 'far' side of 'a'.
        (maxa.z - minb.z), // distance of box 'b' to face on 'near' side of 'a'.
    };

    glm::vec3 normal(0.0f);
    float distance = 0.0f;
    for(int i = 0; i < 6; i ++) {
            // box does not intersect face. So boxes don't intersect at all.
            if(distances[i] < 0.0f){
                return BoxIntersection{false, glm::vec3(0.0f), distance};
            }
            // face of least intersection depth. That's our candidate.
            if((i == 0) || (distances[i] < distance))
            {
                //fcoll = i;
                normal = faces[i];
                distance = distances[i];
            }
        }
    return BoxIntersection{true, normal, distance};
}

glm::mat4 Box::transform() const {
    return transform_;
}

void Box::transform(const glm::mat4 &transform) {
    glm::vec3 position;

    position.x = transform[3][0];
    position.y = transform[3][1];
    position.z = transform[3][2];

    transform_ = glm::translate(glm::mat4(1.0f), position);
}

float Box::volume() const {
    return glm::abs(glm::compMul(max() - min()));
}

float Box::obstruction() const {
    return obstruction_;
}

glm::vec3 Box::size() const {
    return glm::vec3(glm::abs(max_.x-min_.x),
                     glm::abs(max_.y-min_.y),
                     glm::abs(max_.z-min_.z));
}

}
