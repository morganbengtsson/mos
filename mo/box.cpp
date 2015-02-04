#include "box.hpp"

#include <stdexcept>
#include <iostream>
#include <cassert>
#include <array>
#include <glm/gtx/io.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace mo {

Box::Box(){
}

Box::Box(const glm::vec3 & min, const glm::vec3 & max, const glm::mat4 & transform) :
    transform_(transform), min_(min), max_(max) {
    //transform_ = glm::translate(glm::mat4(1.0f), glm::vec3(transform_[3][0], transform_[3][1], transform_[3][2]));
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
    //glm::vec3 p1 = origin + (direction * t1);
    //glm::vec3 p2 = origin + (direction * t2);
    //return intersect(p1, p2);
    // Intersection method from Real-Time Rendering and Essential Mathematics for Games

    glm::mat4 ModelMatrix = transform();
    glm::vec3 ray_origin = origin;
    glm::vec3 ray_direction = direction;
    glm::vec3 aabb_min = min_;
    glm::vec3 aabb_max = max_;

           float tMin = 0.0f;
           float tMax = 100000.0f;

           glm::vec3 OBBposition_worldspace(ModelMatrix[3].x, ModelMatrix[3].y, ModelMatrix[3].z);

           glm::vec3 delta = OBBposition_worldspace - ray_origin;

           // Test intersection with the 2 planes perpendicular to the OBB's X axis
           {
                   glm::vec3 xaxis(ModelMatrix[0].x, ModelMatrix[0].y, ModelMatrix[0].z);
                   float e = glm::dot(xaxis, delta);
                   float f = glm::dot(ray_direction, xaxis);

                   if ( fabs(f) > 0.001f ){ // Standard case

                           float t1 = (e+aabb_min.x)/f; // Intersection with the "left" plane
                           float t2 = (e+aabb_max.x)/f; // Intersection with the "right" plane
                           // t1 and t2 now contain distances betwen ray origin and ray-plane intersections

                           // We want t1 to represent the nearest intersection,
                           // so if it's not the case, invert t1 and t2
                           if (t1>t2){
                                   float w=t1;t1=t2;t2=w; // swap t1 and t2
                           }

                           // tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
                           if ( t2 < tMax )
                                   tMax = t2;
                           // tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
                           if ( t1 > tMin )
                                   tMin = t1;

                           // And here's the trick :
                           // If "far" is closer than "near", then there is NO intersection.
                           // See the images in the tutorials for the visual explanation.
                           if (tMax < tMin )
                                   return RayIntersection(false, glm::vec3(1.0f));

                   }else{ // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
                           if(-e+aabb_min.x > 0.0f || -e+aabb_max.x < 0.0f)
                                   return RayIntersection(false, glm::vec3(1.0f));
                   }
           }


           // Test intersection with the 2 planes perpendicular to the OBB's Y axis
           // Exactly the same thing than above.
           {
                   glm::vec3 yaxis(ModelMatrix[1].x, ModelMatrix[1].y, ModelMatrix[1].z);
                   float e = glm::dot(yaxis, delta);
                   float f = glm::dot(ray_direction, yaxis);

                   if ( fabs(f) > 0.001f ){

                           float t1 = (e+aabb_min.y)/f;
                           float t2 = (e+aabb_max.y)/f;

                           if (t1>t2){float w=t1;t1=t2;t2=w;}

                           if ( t2 < tMax )
                                   tMax = t2;
                           if ( t1 > tMin )
                                   tMin = t1;
                           if (tMin > tMax)
                                   return RayIntersection(false, glm::vec3(1.0f));

                   }else{
                           if(-e+aabb_min.y > 0.0f || -e+aabb_max.y < 0.0f)
                                   return RayIntersection(false, glm::vec3(1.0f));
                   }
           }


           // Test intersection with the 2 planes perpendicular to the OBB's Z axis
           // Exactly the same thing than above.
           {
                   glm::vec3 zaxis(ModelMatrix[2].x, ModelMatrix[2].y, ModelMatrix[2].z);
                   float e = glm::dot(zaxis, delta);
                   float f = glm::dot(ray_direction, zaxis);

                   if ( fabs(f) > 0.001f ){

                           float t1 = (e+aabb_min.z)/f;
                           float t2 = (e+aabb_max.z)/f;

                           if (t1>t2){float w=t1;t1=t2;t2=w;}

                           if ( t2 < tMax )
                                   tMax = t2;
                           if ( t1 > tMin )
                                   tMin = t1;
                           if (tMin > tMax)
                                   return RayIntersection(false, glm::vec3(1.0f));

                   }else{
                           if(-e+aabb_min.z > 0.0f || -e+aabb_max.z < 0.0f)
                                   return RayIntersection(false, glm::vec3(1.0f));
                   }
           }

           float intersection_distance = tMin;
           return RayIntersection(true, glm::vec3(1.0f));
}

RayIntersection Box::intersect(glm::vec3 point1, glm::vec3 point2) {
    glm::vec3 hit(0.0f);
    RayIntersection false_intersection{false, hit};
    glm::vec3 min = min_;
    glm::vec3 max = max_;
    point1 = glm::vec3(glm::inverse(transform()) * glm::vec4(point1, 1.0f));
    point2 = glm::vec3(glm::inverse(transform()) * glm::vec4(point2, 1.0f));
    //glm::vec3 min = this->min();
    //glm::vec3 max = this->max();
    if (point2.x < min.x && point1.x < min.x) {return false_intersection;}
    if (point2.x > max.x && point1.x > max.x) {return false_intersection;}
    if (point2.y < min.y && point1.y < min.y) {return false_intersection;}
    if (point2.y > max.y && point1.y > max.y) {return false_intersection;}
    if (point2.z < min.z && point1.z < min.z) {return false_intersection;}
    if (point2.z > max.z && point1.z > max.z) {return false_intersection;}
    if (point1.x > min.x &&
            point1.x < max.x &&
            point1.y > min.y &&
            point1.y < max.y &&
            point1.z > min.z &&
            point1.z < max.z) {
        hit = point1;
        return RayIntersection{true, hit};
    }
    if ( (intersection( point1.x-min.x, point2.x-min.x, point1, point2, hit) && in_box( hit, min, max, 1 ))
         || (intersection( point1.y-min.y, point2.y-min.y, point1, point2, hit) && in_box( hit, min, max, 2 ))
         || (intersection( point1.z-min.z, point2.z-min.z, point1, point2, hit) && in_box( hit, min, max, 3 ))
         || (intersection( point1.x-max.x, point2.x-max.x, point1, point2, hit) && in_box( hit, min, max, 1 ))
         || (intersection( point1.y-max.y, point2.y-max.y, point1, point2, hit) && in_box( hit, min, max, 2 ))
         || (intersection( point1.z-max.z, point2.z-max.z, point1, point2, hit) && in_box( hit, min, max, 3 ))) {
        return RayIntersection{true, hit};
    }
    return false_intersection;
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
    glm::vec3 maxb = other.max();
    glm::vec3 minb = other.min();

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

glm::mat4 Box::transform() const
{
    return transform_;
}

void Box::transform(const glm::mat4 &transform)
{
    transform_ = transform;
}

}
