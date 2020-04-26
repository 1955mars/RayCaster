#ifndef BOX_H
#define BOX_H

#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/constants.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Hit.h"

#include <iostream>
using namespace std;
using namespace glm;

enum class FaceType
{
    TOP = 0, BOTTOM, FRONT, BACK, LEFT ,RIGHT
};


enum class FaceInterType
{
    NONE = 0, TOWARDS, AWAY
};

struct BoxFace
{
    FaceType type;
    vec4 point;
    vec4 normal;
};

class Box{
    
public: // it would be better to have some kind of protection on members...
	unsigned int num;
    
	vec3 minPos ;
	vec3 maxPos;
    vec3 color;
    
    // rotation matrices
    mat4 rotMat;
    mat4 invRotMat;
    
	// material
	float ambient;
    float diffuse;
	float phong;

    //Faces
    BoxFace boxFaces[6];

    float error = 10e-5;

    RayType rType = RayType::PRIMARY;
    
public:
    Box() {};
    ~Box(){};

    void Draw();

public:
    void FindFace(FaceType type, vec4& point, vec4& dir);
    void CacheFaces();


    FaceInterType RayFaceIntersection(FaceType type, const RayCasted& rayCasted, HitPoint& hitPoint);

    bool RayBoxIntersection(const RayCasted& rayCasted, HitPoint& hitPoint);

    bool isPointinBox(const vec4& iPoint);
    
};

#endif