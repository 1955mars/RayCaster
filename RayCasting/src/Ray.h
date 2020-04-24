#pragma once
#ifndef RAY_H
#define RAY_H


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

#include "Camera.h"

using namespace glm;



class Ray {

public:
    Ray(vec4 p, vec4 dir)
        :point(p), direction(dir)
    {

    }
    ~Ray() {};

private:
    vec4 point;
    vec4 direction;

public:
    void Draw(float t)
    {
        vec4 p1 = point + t * direction;
        glUseProgram(0);
        glDisable(GL_LIGHTING);
        glEnable(GL_DEPTH_TEST);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        
        glBegin(GL_LINES);
        glLineWidth(30.0f);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(point.x, point.y, point.z);
        glVertex3f(p1.x, p1.y, p1.z);

        glVertex3f(0.0f, 0.0f, 8.0f);
        glVertex3f(0.0f, 0.0f, -10.0f);

        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 3.0f, -10.0f);

        //glVertex3f(0.0f, 0.0f, 8.0f);
        //glVertex3f(1.0f, 1.0f, 0.0f);
        //glVertex3f(0.0f, 0.0f, 8.0f);
        //glVertex3f(-1.0f, 1.0f, 0.0f);
        //glVertex3f(0.0f, 0.0f, 8.0f);
        //glVertex3f(1.0f, -1.0f, 0.0f);
        //glVertex3f(0.0f, 0.0f, 8.0f);
        //glVertex3f(-1.0f, -1.0f, 0.0f);

        //glVertex3f(0.0f, 0.0f, 8.0f);
        //glVertex3f(2.0f, 2.0f, 0.0f);
        //glVertex3f(0.0f, 0.0f, 8.0f);
        //glVertex3f(-2.0f, 2.0f, 0.0f);
        //glVertex3f(0.0f, 0.0f, 8.0f);
        //glVertex3f(2.0f, -2.0f, 0.0f);
        //glVertex3f(0.0f, 0.0f, 8.0f);
        //glVertex3f(-2.0f, -2.0f, 0.0f);
        glEnd();

        glPopMatrix();
    };
};

#endif