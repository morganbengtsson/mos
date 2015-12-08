#include "experimental/navmesh.hpp"
#include <glm/gtx/intersect.hpp>

namespace mos {

Navmesh::Navmesh(const Mesh & mesh) : Navmesh(mesh.vertices_begin(),
                                              mesh.vertices_end(),
                                              mesh.elements_begin(),
                                              mesh.elements_end()) {

}

bool Navmesh::intersects(const Box & box) {
    for (auto & face : faces_){
        if (face.intersects(box)){
            return true;
        }
    }
    return false;
}

Navmesh::~Navmesh() {

}

Face::Face(const glm::vec3 & v0,
           const glm::vec3 & v1,
           const glm::vec3 & v2) :
    v0_(v0), v1_(v1), v2_(v2) {
}

bool axistest_x01(const glm::vec3 & v0,
                  const glm::vec3 & v2,
                  const glm::vec3 & boxhalfsize,
                  const float a,
                  const float b,
                  const float fa,
                  const float fb) {
    float p0 = a*v0.y - b*v0.z;
    float p2 = a*v2.y - b*v2.z;

    float min, max;
    if(p0 < p2) {
        min = p0;
        max = p2;
    } else {
        min = p2;
        max = p0;
    }

    float rad = fa * boxhalfsize.y + fb * boxhalfsize.z;

    return !(min > rad || max < -rad);
}



bool axistest_x2(const glm::vec3 & v0,
                 const glm::vec3 & v1,
                 const glm::vec3 & boxhalfsize,
                 const float a,
                 const float b,
                 const float fa,
                 const float fb ) {
    float p0 = a*v0.y - b*v0.z;
    float p1 = a*v1.y - b*v1.z;

    float min, max;
    if(p0 < p1) {
        min = p0;
        max = p1;
    } else {
        min = p1;
        max = p0;
    }

    float rad = fa * boxhalfsize.y + fb * boxhalfsize.z;
    return !(min > rad || max < -rad);
}

bool axistest_y02(const glm::vec3 & v0,
                  const glm::vec3 & v2,
                  const glm::vec3 & boxhalfsize,
                  const float a,
                  const float b,
                  const float fa,
                  const float fb) {
    float p0 = -a*v0.x + b*v0.z;
    float p2 = -a*v2.x + b*v2.z;

    float min, max;
    if(p0 < p2) {
        min = p0;
        max = p2;
    } else {
        min=p2;
        max=p0;
    }
    float rad = fa * boxhalfsize.x + fb * boxhalfsize.z;

    return !(min>rad || max<-rad);
}



bool axistest_y1(const glm::vec3 & v0,
                 const glm::vec3 & v1,
                 const glm::vec3 & boxhalfsize,
                 const float a,
                 const float b,
                 const float fa,
                 const float fb){

    float p0 = -a*v0.x + b*v0.z;
    float p1 = -a*v1.x + b*v1.z;

    float min, max;
    if(p0 < p1) {
        min=p0;
        max=p1;
    } else {
        min=p1;
        max=p0;
    }

    float rad = fa * boxhalfsize.x + fb * boxhalfsize.z;

    return !(min>rad || max<-rad);
}

bool axistest_z12(const glm::vec3 & v1,
                  const glm::vec3 & v2,
                  const glm::vec3 & boxhalfsize,
                  const float a,
                  const float b,
                  const float fa,
                  const float fb) {

    float p1 = a*v1.x - b*v1.y;
    float p2 = a*v2.x - b*v2.y;

    float min, max;
    if(p2 < p1) {
        min = p2;
        max=p1;
    } else {
        min=p1;
        max=p2;
    }
    float rad = fa * boxhalfsize.x + fb * boxhalfsize.y;
    return !(min>rad || max<-rad);
}

bool axistest_z0(const glm::vec3 & v0,
                 const glm::vec3 & v1,
                 const glm::vec3 & boxhalfsize,
                 const float a,
                 const float b,
                 const float fa,
                 const float fb) {
    float p0 = a*v0.x - b*v0.y;
    float p1 = a*v1.x - b*v1.y;

    float min, max;
    if(p0 < p1) {
        min=p0;
        max=p1;
    } else {
        min=p1;
        max=p0;
    }

    float rad = fa * boxhalfsize.x + fb * boxhalfsize.y;

    return !(min>rad || max<-rad);
}

bool Face::intersects(const Box & box) {

    /*
     * This is a variant of:
     * http://fileadmin.cs.lth.se/cs/Personal/Tomas_Akenine-Moller/code/tribox3.txt
     */

     float min,max,p0,p1,p2,rad,fex,fey,fez;

     glm::vec3 normal;

     glm::vec3 boxcenter = box.max() - box.min();

     glm::vec3 v0 = v0_ - boxcenter;
     glm::vec3 v1 = v1_ - boxcenter;
     glm::vec3 v2 = v2_ - boxcenter;

     glm::vec3 e0 = v1 - v0;
     glm::vec3 e1 = v2 - v1;
     glm::vec3 e2 = v0 - v2;


     fex = fabsf(e0.x);
     fey = fabsf(e0.x);
     fez = fabsf(e0.x);

     AXISTEST_X01(e0[Z], e0[Y], fez, fey);
     AXISTEST_Y02(e0[Z], e0[X], fez, fex);
     AXISTEST_Z12(e0[Y], e0[X], fey, fex);


     fex = fabsf(e1[X]);
     fey = fabsf(e1[Y]);
     fez = fabsf(e1[Z]);

     AXISTEST_X01(e1[Z], e1[Y], fez, fey);
     AXISTEST_Y02(e1[Z], e1[X], fez, fex);
     AXISTEST_Z0(e1[Y], e1[X], fey, fex);


     fex = fabsf(e2[X]);
     fey = fabsf(e2[Y]);
     fez = fabsf(e2[Z]);

     AXISTEST_X2(e2[Z], e2[Y], fez, fey);
     AXISTEST_Y1(e2[Z], e2[X], fez, fex);
     AXISTEST_Z12(e2[Y], e2[X], fey, fex);


     /* test in X-direction */

     FINDMINMAX(v0[X],v1[X],v2[X],min,max);

     if(min>boxhalfsize[X] || max<-boxhalfsize[X]) return 0;



     /* test in Y-direction */

     FINDMINMAX(v0[Y],v1[Y],v2[Y],min,max);

     if(min>boxhalfsize[Y] || max<-boxhalfsize[Y]) return 0;



     /* test in Z-direction */

     FINDMINMAX(v0[Z],v1[Z],v2[Z],min,max);

     if(min>boxhalfsize[Z] || max<-boxhalfsize[Z]) return 0;



     /* Bullet 2: */

     /*  test if the box intersects the plane of the triangle */

     /*  compute plane equation of triangle: normal*x+d=0 */

     CROSS(normal,e0,e1);

     // -NJMP- (line removed here)

     if(!planeBoxOverlap(normal,v0,boxhalfsize)) return 0;	// -NJMP-



     return 1;   /* box and triangle overlaps */
}

}
