#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "Box.h"

using namespace glm;

//Cache this function values. They aren't going to change in  a scene
void Box::FindFace(FaceType type, vec4& point, vec4& dir)
{
    if (type == FaceType::TOP)
    {
        vec3 p1(maxPos.x, maxPos.y, minPos.z);
        vec3 p2(minPos.x, maxPos.y, minPos.z);
        vec3 p3(minPos.x, maxPos.y, maxPos.z);
        vec3 normal = normalize(cross(p2 - p1, p3 - p2));
        point = vec4(p1, 1.0f);
        dir = vec4(normal, 0.0f);
    }
    else if (type == FaceType::BOTTOM)
    {
        vec3 p1(maxPos.x, minPos.y, maxPos.z);
        vec3 p2(minPos.x, minPos.y, maxPos.z);
        vec3 p3(minPos.x, minPos.y, minPos.z);
        vec3 normal = normalize(cross(p2 - p1, p3 - p2));
        point = vec4(p1, 1.0f);
        dir = vec4(normal, 0.0f);
    }
    else if (type == FaceType::FRONT)
    {
        vec3 p1(maxPos.x, maxPos.y, maxPos.z);
        vec3 p2(minPos.x, maxPos.y, maxPos.z);
        vec3 p3(minPos.x, minPos.y, maxPos.z);
        vec3 normal = normalize(cross(p2 - p1, p3 - p2));
        point = vec4(p1, 1.0f);
        dir = vec4(normal, 0.0f);
    }
    else if (type == FaceType::BACK)
    {
        vec3 p1(maxPos.x, minPos.y, minPos.z);
        vec3 p2(minPos.x, minPos.y, minPos.z);
        vec3 p3(minPos.x, maxPos.y, minPos.z);
        vec3 normal = normalize(cross(p2 - p1, p3 - p2));
        point = vec4(p1, 1.0f);
        dir = vec4(normal, 0.0f);
    }
    else if (type == FaceType::LEFT)
    {
        vec3 p1(minPos.x, maxPos.y, maxPos.z);
        vec3 p2(minPos.x, maxPos.y, minPos.z);
        vec3 p3(minPos.x, minPos.y, minPos.z);
        vec3 normal = normalize(cross(p2 - p1, p3 - p2));
        point = vec4(p1, 1.0f);
        dir = vec4(normal, 0.0f);
    }
    else if (type == FaceType::RIGHT)
    {
        vec3 p1(maxPos.x, maxPos.y, minPos.z);
        vec3 p2(maxPos.x, maxPos.y, maxPos.z);
        vec3 p3(maxPos.x, minPos.y, maxPos.z);
        vec3 normal = normalize(cross(p2 - p1, p3 - p2));
        point = vec4(p1, 1.0f);
        dir = vec4(normal, 0.0f);
    }

    point = rotMat * point;
    dir = rotMat * dir;

    return;
}


FaceInterType Box::RayFaceIntersection(FaceType type, const RayCasted& rayCasted, HitPoint& hitPoint)
{
    //A = rayPoint, d = rayDir
    //Find the plane of the corresponding face
    vec4& facePoint = boxFaces[(int)type].point; //B
    vec4& faceDir = boxFaces[(int)type].normal; //n

    //(rayDir dot faceDir)
    float dDotN = dot(rayCasted.rayDir, faceDir);
    if (abs(dDotN) < error)
    {
        cout << "Ray and Face are parallel" << endl;
        return FaceInterType::NONE;
    }

    //(B - A) dot n
    float bMinusADotN = dot((facePoint - rayCasted.rayPoint), faceDir);
    float t = bMinusADotN / dDotN;
    hitPoint.point = rayCasted.rayPoint + t * rayCasted.rayDir;
    hitPoint.normalRay = faceDir;

    return dDotN > 0 ? FaceInterType::AWAY : FaceInterType::TOWARDS;
}


bool Box::isPointinBox(const vec4& q)
{
    //vec4 c1 = rotMat * vec4(minPos, 1.0f);
    //vec4 c2 = rotMat * vec4(maxPos, 1.0f);

    vec4 p = invRotMat * q;

    //vec4 newMin = { min(c1.x, c2.x), min(c1.y, c2.y), min(c1.z, c2.z), 1.0f };
    //vec4 newMax = { max(c1.x, c2.x), max(c1.y, c2.y), max(c1.z, c2.z), 1.0f };

    vec4 newMin = vec4(minPos, 1.0f);
    vec4 newMax = vec4(maxPos, 1.0f);

    cout << "CheckPoint: (" << p.x << ", " << p.y << ", " << p.z << endl;
    cout << "newMin: (" << newMin.x << ", " << newMin.y << ", " << newMin.z << endl;
    cout << "newMax: (" << newMax.x << ", " << newMax.y << ", " << newMax.z << endl;

    if( (p.x + error) > newMin.x && (p.x - error) < newMax.x &&
        (p.y + error) > newMin.y && (p.y - error) < newMax.y &&
        (p.z + error) > newMin.z && (p.z - error) < newMax.z)
    {
        return true;
    }

    return false;
}


bool Box::RayBoxIntersection(const RayCasted& rayCasted, HitPoint& hitPoint)
{
    if (isPointinBox(rayCasted.rayPoint))
    {
        if (rayCasted.rayType == RayType::PRIMARY)
        {
            cout << "rayPoint is within or on the box! Not calculating intersections" << endl;
            return false;
        }
        else if (rayCasted.rayType == RayType::SHADOW)
        {
            cout << "Shadow ray is originating from this box!" << endl;
            if (rayCasted.shadowPoint)
            {
                if (dot(rayCasted.shadowPoint->normalRay, rayCasted.rayDir) > -error)
                {
                    cout << "Shadow ray is going out of the box, no intersection with the box" << endl;
                    return false;
                }
                else
                {
                    cout << "Intersection with the box" << endl;
                    return true;
                }
            }
            else
            {
                cout << "Shadow ray doesn't have previous hit point" << endl;
            }
        }
    }

    HitPoint entryPoint{};
    bool initEntryPoint = false;

    HitPoint exitPoint{};
    bool initExitPoint = false;

    FaceType types[6] = { FaceType::TOP, FaceType::BOTTOM, FaceType::FRONT, FaceType::BACK, FaceType::LEFT, FaceType::RIGHT };
    for (FaceType type : types)
    {
        HitPoint interPoint{};
        FaceInterType interType = RayFaceIntersection(type, rayCasted, interPoint);
        if (interType == FaceInterType::NONE)
        {
            cout << "No intersection with face: " << int(type) << endl;
            continue;
        }
        else if (interType == FaceInterType::TOWARDS)
        {
            float d1 = distance2(rayCasted.rayPoint, entryPoint.point);
            float d2 = distance2(rayCasted.rayPoint, interPoint.point);
            float d3 = distance2(rayCasted.rayPoint, exitPoint.point);

            //Update entry point
            if (!initEntryPoint)
            {
                entryPoint = interPoint;
                initEntryPoint = true;
            }
            else
            {
                if (d2 > d1) // If the new intersection point is farther than the previous entry point
                    entryPoint = interPoint;
            }

            //Check if it is closer than the nearest exit point

            if (initExitPoint)
            {
                if (d2 > d3) //If the new entry point is farther than the previous exit point
                {
                    //cout << "new entry point is farther than the previous exit point" << endl;
                    return false;
                }

            }

        }
        else if (interType == FaceInterType::AWAY)
        {
            float d1 = distance2(rayCasted.rayPoint, exitPoint.point);
            float d2 = distance2(rayCasted.rayPoint, interPoint.point);
            float d3 = distance2(rayCasted.rayPoint, entryPoint.point);

            // Update exit point
            if (!initExitPoint)
            {
                exitPoint = interPoint;
                initExitPoint = true;
            }
            else
            {
                if (d2 < d1) //If the new exit point is close than the previous;
                    exitPoint = interPoint;
            }

            //Check if it is farther than the entry point
            if (initEntryPoint)
            {
                if (d2 < d3) // If the new exit point is closer than the entry point
                {
                    //cout << "new exit point is closer than the entry point" << endl;
                    return false;
                }

            }
        }
        cout << "Face : " << (int)type << endl;
        cout << "Intersection point" << endl;
        interPoint.Print();
        cout << "Entry point" << endl;
        entryPoint.Print();
        cout << "Exit point" << endl;
        exitPoint.Print();
    }

    if (initEntryPoint && !isPointinBox(entryPoint.point))
        initEntryPoint = false;

    if (initExitPoint && !isPointinBox(exitPoint.point))
        initExitPoint = false;

    if (rayCasted.rayType == RayType::PRIMARY && !initEntryPoint && !initExitPoint)
    {
        return false;
    }

    hitPoint = entryPoint;
    return true;
}





void Box::CacheFaces()
{
    FaceType types[6] = { FaceType::TOP, FaceType::BOTTOM, FaceType::FRONT, FaceType::BACK, FaceType::LEFT, FaceType::RIGHT };
    for(FaceType type : types)
    {
        int i = (int)type;
        boxFaces[i].type = type;
        FindFace(type, boxFaces[i].point, boxFaces[i].normal);
    }
}

void Box::Draw()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);


    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMultMatrixf((const float*)glm::value_ptr(rotMat));

    glColor3f(color.x, color.y, color.z);

    glBegin(GL_QUADS);

    // Top face (y = 1.0f)
    // Counter-clockwise (CCW) with normal pointing out
    glVertex3f(maxPos.x, maxPos.y, minPos.z);
    glVertex3f(minPos.x, maxPos.y, minPos.z);
    glVertex3f(minPos.x, maxPos.y, maxPos.z);
    glVertex3f(maxPos.x, maxPos.y, maxPos.z);

    // Bottom face (y = -1.0f)
    glVertex3f(maxPos.x, minPos.y, maxPos.z);
    glVertex3f(minPos.x, minPos.y, maxPos.z);
    glVertex3f(minPos.x, minPos.y, minPos.z);
    glVertex3f(maxPos.x, minPos.y, minPos.z);

    // Front face  (z = 1.0f)
    glVertex3f(maxPos.x, maxPos.y, maxPos.z);
    glVertex3f(minPos.x, maxPos.y, maxPos.z);
    glVertex3f(minPos.x, minPos.y, maxPos.z);
    glVertex3f(maxPos.x, minPos.y, maxPos.z);

    // Back face (z = -1.0f)
    glVertex3f(maxPos.x, minPos.y, minPos.z);
    glVertex3f(minPos.x, minPos.y, minPos.z);
    glVertex3f(minPos.x, maxPos.y, minPos.z);
    glVertex3f(maxPos.x, maxPos.y, minPos.z);

    // Left face (x = -1.0f)
    glVertex3f(minPos.x, maxPos.y, maxPos.z);
    glVertex3f(minPos.x, maxPos.y, minPos.z);
    glVertex3f(minPos.x, minPos.y, minPos.z);
    glVertex3f(minPos.x, minPos.y, maxPos.z);

    // Right face (x = 1.0f)
    glVertex3f(maxPos.x, maxPos.y, minPos.z);
    glVertex3f(maxPos.x, maxPos.y, maxPos.z);
    glVertex3f(maxPos.x, minPos.y, maxPos.z);
    glVertex3f(maxPos.x, minPos.y, minPos.z);
    glEnd();

    glPopMatrix();
};