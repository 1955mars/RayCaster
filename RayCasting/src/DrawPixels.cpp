//#include <GL/glut.h>
//
//int g_winWidth = 640;
//int g_winHeight = 480;
//
//float vertices[4 * 2] = { 0,   0,
//                          640, 0,
//                          640, 480,
//                          0,   480 };
//float texCoords[4 * 2] = { 0, 0,
//                         1, 0,
//                         1, 1,
//                         0, 1 };
//
//GLuint glTexID = -1;
//
//unsigned char* imagedata;
//
//void createTexture()
//{
//    imagedata = new unsigned char[g_winWidth * g_winHeight * 3];
//
//    // assign red color (255, 0 , 0) to each pixel
//    for (int i = 0; i < g_winWidth * g_winHeight; i++)
//    {
//        imagedata[i * 3 + 0] = 255; // R
//        imagedata[i * 3 + 1] = 0;   // G
//        imagedata[i * 3 + 2] = 0;   // B
//    }
//
//    glGenTextures(1, &glTexID);
//    glBindTexture(GL_TEXTURE_2D, glTexID);
//
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//
//    // send the imagedata (on CPU) to the GPU memory at glTexID (glTexID is a GPU memory location index)
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, g_winWidth, g_winHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imagedata);
//}
//
//void initGLStuff(void)
//{
//    createTexture();
//}
//
//void drawPlane()
//{
//    glDisable(GL_LIGHTING);
//    glEnable(GL_DEPTH_TEST);
//
//    glMatrixMode(GL_MODELVIEW);
//    glPushMatrix();
//
//    glEnable(GL_TEXTURE_2D);
//    glBindTexture(GL_TEXTURE_2D, glTexID); // use the texture on the quad 
//    glBegin(GL_QUADS);
//    glTexCoord2fv(texCoords + 2 * 0); glVertex2fv(vertices + 2 * 0);
//    glTexCoord2fv(texCoords + 2 * 1); glVertex2fv(vertices + 2 * 1);
//    glTexCoord2fv(texCoords + 2 * 2); glVertex2fv(vertices + 2 * 2);
//    glTexCoord2fv(texCoords + 2 * 3); glVertex2fv(vertices + 2 * 3);
//    glEnd();
//    glDisable(GL_TEXTURE_2D);
//
//    glPopMatrix();
//}
//
//void display(void)
//{
//    glClearColor(1.0, 1.0, 1.0, 1.0);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//    // if need to update imagedata, do it here, before displaying it as a texture on the quad 
//    drawPlane();
//    glutSwapBuffers();
//}
//
//void reshape(int w, int h)
//{
//    g_winWidth = w;
//    g_winHeight = h;
//
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
//
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    gluOrtho2D(0.0, 640.0, 0.0, 480.0);
//
//    glViewport(0, 0, (GLsizei)g_winWidth, (GLsizei)g_winHeight);
//}
//
//void keyboard(unsigned char key, int x, int y)
//{
//    switch (key) {
//    case 27:
//        exit(0);
//        break;
//    }
//}
//
//int main(int argc, char* argv[])
//{
//    glutInit(&argc, argv);
//    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
//    glutInitWindowSize((int)g_winWidth, (int)g_winHeight);
//
//    glutCreateWindow("Draw Pixels");
//
//    initGLStuff();
//
//    glutReshapeFunc(reshape);
//    glutDisplayFunc(display);
//    glutKeyboardFunc(keyboard);
//    glutMainLoop();
//
//    return 0;
//}
