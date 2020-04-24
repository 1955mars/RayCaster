#ifndef SPHERE_H
#define SPHERE_H

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

enum class PointLocation
{
    OUTSIDE = 0, SURFACE, INSIDE
};

class Sphere{

public: // it would be better to have some kind of protection on members...

	vec3 pos;
    float radius;
    vec3 color;
    
	// material
	float ambient;
    float diffuse;
	float phong;
    
    float error = 10e-3;
public:
    Sphere(){};
    ~Sphere(){};
    
    bool RaySphereIntersection(const RayCasted& rayCasted, HitPoint& hitPoint);

    PointLocation isPointinSphere(const vec4& iPoint);

    void Draw()
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDisable(GL_LIGHTING);
        
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glTranslatef(pos.x, pos.y, pos.z);
        glColor3f(color.x,color.y, color.z);
        glutWireSphere(radius, 10, 10);
        
        glPopMatrix();
    };
};

#endif