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

#include "Sphere.h"
#include "Box.h"
#include "Light.h"
#include "Ray.h"

#include "Text.h"

#include <fstream>
#include <iostream>
using namespace std;
using namespace glm;

int g_winWidth  = 640;
int g_winHeight = 480;

Camera g_cam;
Text g_text;
unsigned char g_keyStates[256];

GLfloat light0_pos[] = {0.0f, 5.0f, 5.0f, 0.0f};
GLfloat light0_Amb[] = {0.4f, 0.3f, 0.3f, 1.0f};
GLfloat light0_Diff[] = {0.8f, 0.8f, 0.7f, 1.0f};
GLfloat light0_Spec[] = {0.9f, 0.9f, 0.9f, 1.0f};

vec3 light0Ambient = vec3(light0_Amb[0], light0_Amb[1], light0_Amb[2]);
vec3 litht0Diffuse = vec3(light0_Diff[0], light0_Diff[1], light0_Diff[2]);
vec3 light0Spec = vec3(light0_Spec[0], light0_Spec[1], light0_Spec[2]);

const char dataFile[128] = "geoData/geo.txt";

unsigned int g_box_num;
Box* g_boxes;

unsigned int g_sphere_num;
Sphere * g_spheres;

Light g_light;

float* imagedata = new float[g_winWidth * g_winHeight * 3];
GLuint glTexID = -1;
float vertices[4 * 2] = { 0, 0,
                           640, 0,
                           640, 480,
                           0,  480 };
float texCoords[4 * 2] = { 0, 0,
                         1, 0,
                         1, 1,
                         0, 1 };

enum class IntersectType
{
    NONE = 0, INTERSECT, LIGHT
};

void LoadConfigFile(const char* pFilePath)
{
    const unsigned int CAMERA= 0;
    const unsigned int LIGHT= 1;
    const unsigned int SPHERE= 2;
    const unsigned int BOX = 3;
    
    vec3 rot;
    fstream filestr;
    filestr.open (pFilePath, fstream::in);
    if(!filestr.is_open()){
        cout<<"'"<<pFilePath<<"'"<<", geo file does not exsit!"<<endl;
        filestr.close();
        return;
    }
    
    char objType[80];
    char attrType[80];
    unsigned int objState = -1;
    
    bool loop = true;
    while(loop){
        filestr>>objType;
        if(!strcmp(objType, "Camera:")) objState=CAMERA;
        else if(!strcmp(objType, "Light:")) objState = LIGHT;
        else if(!strcmp(objType, "Sphere:")) objState = SPHERE;
        else if(!strcmp(objType, "Box:")) objState = BOX;
        switch(objState){
            case CAMERA:
                float e_x, e_y, e_z;
                float l_x, l_y, l_z;
                float near_plane;
                
                for(int i=0; i<3; i++){
                    filestr>>attrType;
                    if(!strcmp(attrType, "eye:")){
                        filestr>>e_x;
                        filestr>>e_y;
                        filestr>>e_z;
                    }
                    else if(!strcmp(attrType, "lookat:")){
                        filestr>>l_x;
                        filestr>>l_y;
                        filestr>>l_z;
                    }
                    else if(!strcmp(attrType, "near_plane:")) filestr>>near_plane;
                }
                g_cam.set(e_x, e_y, e_z, l_x, l_y, l_z, g_winWidth, g_winHeight, 45.0f, near_plane, 1000.0f);
                break;
            case LIGHT:
                filestr>>attrType;
                if(!strcmp(attrType, "position:")){
                    filestr>>g_light.pos.x;
                    filestr>>g_light.pos.y;
                    filestr>>g_light.pos.z;
                }
                filestr>>attrType;
                if(!strcmp(attrType, "color:")){
                    filestr>>g_light.color.x;
                    filestr>>g_light.color.y;
                    filestr>>g_light.color.z;
                }
                filestr>>attrType;
                if(!strcmp(attrType, "intensity:")){
                    filestr>>g_light.intensity;
                }
                break;
            case SPHERE:
                filestr>>attrType;
                if(!strcmp(attrType, "num:"))
                    filestr>>g_sphere_num;
                if(g_sphere_num > 0) {
                    g_spheres = new Sphere[g_sphere_num];
                    
                    for(int i=0; i<g_sphere_num; i++){
                    filestr>>attrType;
                    if(!strcmp(attrType, "position:")){
                        filestr>>g_spheres[i].pos.x; filestr>>g_spheres[i].pos.y;  filestr>>g_spheres[i].pos.z;
                    }
                    filestr>>attrType;
                    if(!strcmp(attrType, "radius:")){
                        filestr>>g_spheres[i].radius;
                    }
                    filestr>>attrType;
                    if(!strcmp(attrType, "color:")){
                        filestr>>g_spheres[i].color.x; filestr>>g_spheres[i].color.y;  filestr>>g_spheres[i].color.z;
                    }
                    filestr>>attrType;
                    if(!strcmp(attrType, "ambient:"))	filestr>>g_spheres[i].ambient;
                    filestr>>attrType;
                    if(!strcmp(attrType, "diffuse:"))	filestr>>g_spheres[i].diffuse;
                    filestr>>attrType;
                    if(!strcmp(attrType, "phong:")) 	filestr>>g_spheres[i].phong;
                    
                }
                }
                break;
            case BOX:
                filestr>>attrType;
                if(!strcmp(attrType, "num:"))
                    filestr>>g_box_num;
                
                if(g_box_num > 0) {
                    g_boxes = new Box[g_box_num];
                    for(int i=0; i<g_box_num; i++){
                        filestr>>attrType;
                        if(!strcmp(attrType, "conner_position:")){
                            filestr>>g_boxes[i].minPos.x;
                            filestr>>g_boxes[i].minPos.y;
                            filestr>>g_boxes[i].minPos.z;
                            filestr>>g_boxes[i].maxPos.x;
                            filestr>>g_boxes[i].maxPos.y;
                            filestr>>g_boxes[i].maxPos.z;
                        }
                        
                        filestr>>attrType;
                        if(!strcmp(attrType, "color:")){
                            filestr>>g_boxes[i].color.x;
                            filestr>>g_boxes[i].color.y;
                            filestr>>g_boxes[i].color.z;
                        }
                        filestr>>attrType;
                        if(!strcmp(attrType, "rotate:")){
                            filestr>>rot.x;
                            filestr>>rot.y;
                            filestr>>rot.z;
							//Convert to radians
							rot.x *= 3.14159265f / 180.0f;
							rot.y *= 3.14159265f / 180.0f;
							rot.z *= 3.14159265f / 180.0f;

                            mat4 m (1.0f);
                            
                            
                            // rotation order is zyx
                            m = rotate(m, rot.z, vec3(0, 0, 1));
                            m = rotate(m, rot.y, vec3(0, 1, 0));
                            m = rotate(m, rot.x, vec3(1, 0, 0));
                            
                            //cout<<glm::to_string(m)<<endl;
                            
                            g_boxes[i].rotMat = m;
                            g_boxes[i].invRotMat = inverse(m);
                            g_boxes[i].CacheFaces();
                        }
                        filestr>>attrType;
                        if(!strcmp(attrType, "ambient:"))	filestr>>g_boxes[i].ambient;
                        filestr>>attrType;
                        if(!strcmp(attrType, "diffuse:"))	filestr>>g_boxes[i].diffuse;
                        filestr>>attrType;
                        if(!strcmp(attrType, "phong:"))	filestr>>g_boxes[i].phong;
                    }
                    loop = false;
                }
                break;
        }
    }
}


void initialization() 
{    
    //g_cam.set(3.0f, 4.0f, 3.0f, 0.0f, 0.0f, 0.0f, g_winWidth, g_winHeight);

    LoadConfigFile(dataFile);
    
	g_text.setColor(0.0f, 0.0f, 0.0f);
}

void createTexture()
{
    glGenTextures(1, &glTexID);
    glBindTexture(GL_TEXTURE_2D, glTexID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, g_winWidth, g_winHeight, 0, GL_RGB, GL_FLOAT, imagedata);

}


/****** GL callbacks ******/
void initialGL()
{    
    createTexture();

	glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_Amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_Diff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_Spec);


	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glPolygonMode(GL_FRONT, GL_FILL);

	
	glClearColor (1.0f, 1.0f, 1.0f, 0.0f);
	glShadeModel(GL_SMOOTH);

	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();

}

void idle()
{
    // adding stuff to update at runtime ....
    
    g_cam.keyOperation(g_keyStates, g_winWidth, g_winHeight);
}


void SendTextureData(unsigned char* imagedata)
{
    glBindTexture(GL_TEXTURE_2D, glTexID);
    // send the imagedata (on CPU) to the GPU memory at glTexID (glTexID is a GPU memory location index)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, g_winWidth, g_winHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imagedata);
}

void drawTexture(float* texCoords, float* vertices)
{
    glDisable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, glTexID); // use the texture on the quad 

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, g_winWidth, g_winHeight, 0, GL_RGB, GL_FLOAT, imagedata);

    glBegin(GL_QUADS);
    glTexCoord2fv(texCoords + 2 * 0); glVertex2fv(vertices + 2 * 0);
    glTexCoord2fv(texCoords + 2 * 1); glVertex2fv(vertices + 2 * 1);
    glTexCoord2fv(texCoords + 2 * 2); glVertex2fv(vertices + 2 * 2);
    glTexCoord2fv(texCoords + 2 * 3); glVertex2fv(vertices + 2 * 3);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

void printPos(const vec3& pos)
{
    cout << "       (" << pos.x << ", " << pos.y << ", " << pos.z << ")" << endl;
}

void printPos(const vec4& pos)
{
    cout << "       (" << pos.x << ", " << pos.y << ", " << pos.z << ")" << endl;
}

IntersectType intersect(const RayCasted& rayCasted, HitPoint& hitPoint)
{
    HitPoint closestPoint{};
    float entryPointDistance = 0.0f;
    bool hasFoundOne = false;

    //Boxes
    for (int i = 0; i < g_box_num; i++)
    {
        HitPoint entryPoint{};

        bool iStatus = g_boxes[i].RayBoxIntersection(rayCasted, entryPoint);
        if (iStatus)
        {
            if (rayCasted.rayType == RayType::SHADOW)
            {
                //cout << "Shadow ray has intersection with geometry!" << endl;
                return IntersectType::INTERSECT;
            }
                
            //cout << "Intersection with box : " << i << endl;
            //entryPoint.Print();

            if (!hasFoundOne)
            {
                closestPoint = entryPoint;
                closestPoint.objectHit = (void*)(g_boxes + i);
                closestPoint.objType = ObjectType::BOX;

                entryPointDistance = distance2(rayCasted.rayPoint, entryPoint.point);
                hasFoundOne = true;
            }
            else
            {
                float newDistance = distance2(rayCasted.rayPoint, entryPoint.point);
                if (newDistance < entryPointDistance)
                {
                    closestPoint = entryPoint; 
                    closestPoint.objectHit = (void*)(g_boxes + i);
                    closestPoint.objType = ObjectType::BOX;

                    entryPointDistance = newDistance;
                }
            } 
            //cout << "EntryPointDistance: " << entryPointDistance << endl;
        }
        else
        {
            //cout << "No intersection with box " << i << endl;
        }
    }

    //TODO: Spheres
    for (int i = 0; i < g_sphere_num; i++)
    {
        HitPoint entryPoint{};

        bool iStatus = g_spheres[i].RaySphereIntersection(rayCasted, entryPoint);
        if (iStatus)
        {
            if (rayCasted.rayType == RayType::SHADOW)
            {
                //cout << "Shadow ray has intersection with geometry!" << endl;
                return IntersectType::INTERSECT;
            }

            //cout << "Intersection with Sphere: " << i << endl;
            //entryPoint.Print();

            if (!hasFoundOne)
            {
                closestPoint = entryPoint;
                closestPoint.objectHit = (void*)(g_spheres + i);
                closestPoint.objType = ObjectType::SPHERE;

                entryPointDistance = distance2(rayCasted.rayPoint, entryPoint.point);
                hasFoundOne = true;
            }
            else
            {
                float newDistance = distance2(rayCasted.rayPoint, entryPoint.point);
                if (newDistance < entryPointDistance)
                {
                    closestPoint = entryPoint;
                    closestPoint.objectHit = (void*)(g_spheres + i);
                    closestPoint.objType = ObjectType::SPHERE;

                    entryPointDistance = newDistance;
                }
            }
            //cout << "EntryPointDistance: " << entryPointDistance << endl;
        }
        else
        {
            //cout << "No intersection with sphere " << i << endl;
        }
    }


    if (!hasFoundOne)
    {
        if (rayCasted.rayType == RayType::SHADOW)
        {
            //cout << "Shadow ray hasn't intersected with any geometry" << endl;
            return IntersectType::NONE;
        }

        //See if the ray is directly coming from the light source
        //vec4 lightDir = vec4(g_light.pos, 0.0f);
        //if (dot(lightDir, rayDir) == -1);

        return IntersectType::NONE;
    }


    hitPoint = closestPoint;
    return IntersectType::INTERSECT;
}

struct PhongData
{
    vec4 shadowRay;

};

vec4 PhongColor(vec4& shadowRay, vec4& invPriRay, vec4& normal, vec4& reflRay)
{
    return vec4(0.0f);
}

void rayTracer(const vec4& rayPoint, const vec4& rayDir, vec3& rColor)
{
    //Find closest intersection point in all the geometry
    vec4 iPoint;

    RayCasted rayCasted{};
    rayCasted.rayPoint = rayPoint;
    rayCasted.rayDir = rayDir;

    HitPoint hitPoint{};

    IntersectType iStatus = intersect(rayCasted, hitPoint);

    if (iStatus == IntersectType::INTERSECT)
    {
        //cout << "Primary ray intersection point found" << endl;
        //hitPoint.Print();

        RayCasted shadowRay{};
        shadowRay.rayType = RayType::SHADOW;
        shadowRay.rayPoint = hitPoint.point;
        shadowRay.rayDir = normalize(vec4(g_light.pos, 1.0f) - hitPoint.point); //hit point to light
        HitPoint tempPoint = hitPoint;
        shadowRay.shadowPoint = &tempPoint;

        HitPoint shadowIntersect{};
        IntersectType shadowStatus = intersect(shadowRay, shadowIntersect);

        float ambient;
        float diffuse;
        float phong;
        vec3 objColor;

        if (hitPoint.objType == ObjectType::BOX)
        {
            Box* hitBox = (Box*)hitPoint.objectHit;
            ambient = hitBox->ambient;
            diffuse = hitBox->diffuse;
            phong = hitBox->phong;
            objColor = hitBox->color;
        }
        else if (hitPoint.objType == ObjectType::SPHERE)
        {
            Sphere* hitSphere = (Sphere*)hitPoint.objectHit;
            ambient = hitSphere->ambient;
            diffuse = hitSphere->diffuse;
            phong = hitSphere->phong;
            objColor = hitSphere->color;
        }

        //Calculate Blinn Phong light model
        vec3 sRay = vec3(shadowRay.rayDir); //Hit point to light
        vec3 vRay = vec3(normalize(rayPoint - hitPoint.point)); // hit point to eye
        vec3 nRay = vec3(hitPoint.normalRay); //Normal
        vec3 rRay = normalize((2.0f * nRay * dot(nRay, sRay)) - sRay);

        vec3 hRay = normalize(sRay + vRay); //Half Ray - Blinn Phong Model

        float lightDistance = distance(vec4(g_light.pos, 1.0f) , hitPoint.point);

        //Phong Model
        //vec3 Ia = ambient * light0Ambient;
        vec3 Ia = light0Ambient;
        vec3 Id = (diffuse * litht0Diffuse * max(0.0f, dot(sRay, nRay)));
        
        vec3 Is = phong * light0Spec * pow(max(0.0f, dot(rRay, vRay)), 50);

        //vec3 Is = (phong * vec3(light0Spec) * pow(max(dot(nRay, hRay), 0.0f), 100));

        rColor = ((Ia + Id) * objColor + Is) * g_light.color;

        //printPos(rColor);

        if (shadowStatus == IntersectType::NONE)
        {
            //cout << "Seconday ray no intersection! Do Bling Phong computation here!" << endl;

            return;
        }
        else
        {
            //cout << "Secondary ray has intersection" << endl;

            rColor = 0.25f * rColor;
            return;
        }
    }
    else if (iStatus == IntersectType::NONE)
    {
        //cout << "Primary ray doesn't have any intersection" << endl;

        //Return background color here
        rColor = vec3(0.0f);
        return;
    }

}



void rayCast()
{
    vec4 screenUpDirection = normalize(g_cam.ntl - g_cam.nbl);
    vec4 screenRightDirection = normalize(g_cam.nbr - g_cam.nbl);


    vec4 deltaUp = (1.0f / g_winHeight) * (g_cam.ntl - g_cam.nbl);
    vec4 deltaRight = (1.0f / g_winWidth) * (g_cam.nbr - g_cam.nbl);

    vec4 basePixelPos = g_cam.nbl;
    
    float depth = (g_cam.eye.z - g_cam.near_plane);

    vertices[0 * 2 + 0] = g_cam.nbl.x * depth; vertices[0 * 2 + 1] = g_cam.nbl.y * depth;
    vertices[1 * 2 + 0] = g_cam.nbr.x * depth; vertices[1 * 2 + 1] = g_cam.nbr.y * depth;
    vertices[2 * 2 + 0] = g_cam.ntr.x * depth; vertices[2 * 2 + 1] = g_cam.ntr.y * depth;
    vertices[3 * 2 + 0] = g_cam.ntl.x * depth; vertices[3 * 2 + 1] = g_cam.ntl.y * depth;

    //texCoords[0 * 2 + 0] = vertices[0 * 2 + 0]; texCoords[0 * 2 + 1] = vertices[0 * 2 + 1];
    //texCoords[1 * 2 + 0] = vertices[1 * 2 + 0]; texCoords[1 * 2 + 1] = vertices[1 * 2 + 1];
    //texCoords[2 * 2 + 0] = vertices[2 * 2 + 0]; texCoords[2 * 2 + 1] = vertices[2 * 2 + 1];
    //texCoords[3 * 2 + 0] = vertices[3 * 2 + 0]; texCoords[3 * 2 + 1] = vertices[3 * 2 + 1];

    int k = 0;

    for (int i = 0; i < g_winHeight; i++)
    {
        vec4 startRowPixel = basePixelPos + (i * 1.0f * deltaUp * screenUpDirection);
        for (int j = 0; j < g_winWidth; j++)
        {
            vec4 pos = startRowPixel + (j * 1.0f * deltaRight * screenRightDirection);
            vec4 dir = normalize(pos - g_cam.eye);
            vec3 pixelColor(0.0f);
            rayTracer(g_cam.eye, dir, pixelColor);
            imagedata[k * 3 + 0] = (pixelColor.r);// * 255.0f);
            imagedata[k * 3 + 1] = (pixelColor.g);// * 255.0f);
            imagedata[k * 3 + 2] = (pixelColor.b);// * 255.0f);
            k++;
        }
    }

}


void display()
{
    //glFinish();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_pos); // commenting out this line to make object always lit up in front of the cam. 

    rayCast();
    drawTexture(texCoords, vertices);

    //vec4 rColor(0.0f);
    //rayTracer(g_cam.eye, normalize(vec4(0.0f, 0.0f, 0.0f, 1.0f) - g_cam.eye), rColor);
    //cout << "Color returned for the ray" << endl;
    //printPos(rColor);


    //Ray ray(vec4(0.0), vec4(0.0f));
    //ray.Draw(10.0f);

     //drae sphere and box
    //for (int i=1; i<g_sphere_num; i++)
    //    g_spheres[i].Draw();
    //for (int i=0; i<g_box_num; i++)
    //    g_boxes[i].Draw();

    // displaying the camera
    //g_cam.drawGrid();
    //g_cam.drawCoordinateOnScreen(g_winWidth, g_winHeight);
    //g_cam.drawCoordinate();

	// displaying the text
	//if(g_cam.isFocusMode()) {
 //       string str = "Cam mode: Focus";
	//	g_text.draw(10, 30, const_cast<char*>(str.c_str()), g_winWidth, g_winHeight);
	//} else if(g_cam.isFPMode()) {
 //       string str = "Cam mode: FP";
	//	g_text.draw(10, 30, const_cast<char*>(str.c_str()), g_winWidth, g_winHeight);
	//}

	//char s[128];
	//g_text.draw(10, 50, s, g_winWidth, g_winHeight);


    glutSwapBuffers();
}

void reshape(int w, int h)
{
	g_winWidth = w;
	g_winHeight = h;
    //cout << "Width = " << w << " Height = " << h << endl;
	if (h == 0) {
		h = 1;
	}
	g_cam.setProj(g_winWidth, g_winHeight);
    g_cam.setModelView();
    glViewport(0, 0, w, h);
}

void mouse(int button, int state, int x, int y)
{
    g_cam.mouseClick(button, state, x, y, g_winWidth, g_winHeight);
}

void motion(int x, int y)
{
    g_cam.mouseMotion(x, y, g_winWidth, g_winHeight);
    //cout << " Eye position : " << g_cam.eye.x << "," << g_cam.eye.y << "," << g_cam.eye.z << endl;
}

void keyup(unsigned char key, int x, int y)
{
    g_keyStates[key] = false;
}

void keyboard(unsigned char key, int x, int y)
{
    g_keyStates[key] = true;
	switch(key) { 
		case 27:
			exit(0);
			break;
        case 'c': // switch cam control mode
            g_cam.switchCamMode();
			glutPostRedisplay();
            break;
        case ' ':
            g_cam.PrintProperty();
            break;

	}
}


int main(int argc, char **argv) 
{
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(g_winWidth, g_winHeight);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Ray Casting");
	
	glewInit();
	initialGL();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
    glutKeyboardUpFunc(keyup);
    glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	initialization();
	
    glutMainLoop();
    return EXIT_SUCCESS;
}