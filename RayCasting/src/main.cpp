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

const char dataFile[128] = "geoData/geo.txt";

unsigned int g_box_num;
Box* g_boxes;

unsigned int g_sphere_num;
Sphere * g_spheres;

Light g_light;

unsigned char* imagedata = new unsigned char[g_winWidth * g_winHeight * 3];
GLuint glTexID = -1;
float vertices[4 * 2] = { -320, -240,
                           320, -240,
                           320,  240,
                          -320,  240 };
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

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, g_winWidth, g_winHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imagedata);

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

void drawTexture(float* texCoords, float* vertices, unsigned char* imagedata)
{
    glDisable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, glTexID); // use the texture on the quad 
    glBegin(GL_QUADS);
    glTexCoord2fv(texCoords + 2 * 0); glVertex2fv(vertices + 2 * 0);
    glTexCoord2fv(texCoords + 2 * 1); glVertex2fv(vertices + 2 * 1);
    glTexCoord2fv(texCoords + 2 * 2); glVertex2fv(vertices + 2 * 2);
    glTexCoord2fv(texCoords + 2 * 3); glVertex2fv(vertices + 2 * 3);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}


void printPos(vec4& pos)
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
                cout << "Shadow ray has intersection with geometry!" << endl;
                return IntersectType::INTERSECT;
            }
                
            cout << "Intersection with box : " << i << endl;
            entryPoint.Print();

            if (!hasFoundOne)
            {
                closestPoint = entryPoint;
                entryPointDistance = distance2(rayCasted.rayPoint, entryPoint.point);
                hasFoundOne = true;
            }
            else
            {
                float newDistance = distance2(rayCasted.rayPoint, entryPoint.point);
                if (newDistance < entryPointDistance)
                {
                    closestPoint = entryPoint;
                    entryPointDistance = newDistance;
                }
            } 
            //cout << "EntryPointDistance: " << entryPointDistance << endl;
        }
        else
        {
            cout << "No intersection with box " << i << endl;
        }
    }

    //TODO: Spheres


    if (!hasFoundOne)
    {
        if (rayCasted.rayType == RayType::SHADOW)
        {
            cout << "Shadow ray hasn't intersected with any geometry" << endl;
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
    vec3 Ia;
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
        cout << "Primary ray intersection point found" << endl;
        hitPoint.Print();

        RayCasted shadowRay{};
        shadowRay.rayType = RayType::SHADOW;
        shadowRay.rayPoint = hitPoint.point;
        shadowRay.rayDir = normalize(vec4(g_light.pos, 1.0f) - hitPoint.point); //hit point to light
        shadowRay.shadowPoint = &hitPoint;

        HitPoint shadowIntersect{};
        IntersectType shadowStatus = intersect(shadowRay, shadowIntersect);

        if (shadowStatus == IntersectType::NONE)
        {
            cout << "Seconday ray no intersection" << endl;
            
            //Calculate Blinn Phong light model
            vec4 sRay = shadowRay.rayDir; //Hit point to light
            vec4 vRay = normalize(rayPoint - hitPoint.point); // hit point to eye
            vec4 nRay = hitPoint.normalRay; //Normal
            vec4 rRay = (2.0f * nRay * dot(nRay, sRay)) - sRay;

      
            

            rColor = vec3(1.0f, 1.0f, 0.0f);
            return;
        }
        else
        {
            cout << "Secondary ray has intersection" << endl;

            rColor = vec3(0.0f);
            return;
        }
    }
    else if (iStatus == IntersectType::NONE)
    {
        cout << "Primary ray doesn't have any intersection" << endl;

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

    int k = 0;

    for (int i = 0; i < g_winHeight; i++)
    {
        vec4 startRowPixel = basePixelPos + (i * 1.0f * deltaUp * screenUpDirection);
        for (int j = 0; j < g_winWidth; j++)
        {
            vec4 pos = startRowPixel + (j * 1.0f * deltaRight * screenRightDirection);
            vec4 dir = normalize(pos - g_cam.eye);
            vec3 pixelColor(0.0f);
            rayTracer(pos, dir, pixelColor);
            imagedata[k * 3 + 0] = (pixelColor.r) * 255;
            imagedata[k * 3 + 1] = (pixelColor.g) * 255;
            imagedata[k * 3 + 2] = (pixelColor.b) * 255;
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

    //rayCast();
    //drawTexture(texCoords, vertices, imagedata);

    vec3 rColor(0.0f);
    rayTracer(g_cam.eye, normalize(vec4(0.0f, 0.0f, 0.0f, 1.0f) - g_cam.eye), rColor);

    Ray ray(vec4(0.0), vec4(0.0f));
    ray.Draw(10.0f);

     //drae sphere and box
    //for (int i=0; i<g_sphere_num; i++)
    //    g_spheres[i].Draw();
    for (int i=0; i<g_box_num; i++)
        g_boxes[i].Draw();

    // displaying the camera
    g_cam.drawGrid();
    g_cam.drawCoordinateOnScreen(g_winWidth, g_winHeight);
    g_cam.drawCoordinate();

	// displaying the text
	if(g_cam.isFocusMode()) {
        string str = "Cam mode: Focus";
		g_text.draw(10, 30, const_cast<char*>(str.c_str()), g_winWidth, g_winHeight);
	} else if(g_cam.isFPMode()) {
        string str = "Cam mode: FP";
		g_text.draw(10, 30, const_cast<char*>(str.c_str()), g_winWidth, g_winHeight);
	}

	char s[128];
	g_text.draw(10, 50, s, g_winWidth, g_winHeight);


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