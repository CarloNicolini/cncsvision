// This file is part of CNCSVision, a computer vision related library
// This software is developed under the grant of Italian Institute of Technology
//
// Copyright (C) 2010-2014 Carlo Nicolini <carlo.nicolini@iit.it>
//
//
// CNCSVision is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// Alternatively, you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CNCSVision is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License or the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License and a copy of the GNU General Public License along with
// CNCSVision. If not, see <http://www.gnu.org/licenses/>.


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>

#include "IncludeGL.h"
#include "VRCamera.h"
#include "VolumetricSurfaceIntersection.h"
#include "LatestCalibration.h"

#include "Arcball.h"
#include "GLText.h"
#include "Icosphere.h"

#include "GLTexture.h"

using std::cerr;
using std::endl;
using std::string;

//=================================//
// EXPERIMENTS FUNCTION PROTOTYPES //
//=================================//
void drawTrial();
void drawGLScene();

//=================================//
//    OPENGL Graphics Variables    //
//=================================//
const bool GameMode=false;
const bool IsStereo=false;

// Camera and visualization setup
const double focalDistance=-418.5;
const double interocularDistance=65;
double eyeDistance=0.0;
VRCamera cam;
Arcball arcball;

//=================================//
//  EXPERIMENTAL VISUAL STIMULI    //
//=================================//
#define TEXTURE_SIZE 768
VolumetricSurfaceIntersection surface(TEXTURE_SIZE,TEXTURE_SIZE,TEXTURE_SIZE);
#ifdef __APPLE__
char vertexShaderFile[] = "/Users/rs/workspace/cncsvision/experiments/texturing/vertex_perlin.vert";
char fragmentShaderFile[] = "/Users/rs/workspace/cncsvision/experiments/texturing/fragment_perlin.frag";
char vertexShaderFloorFile[] = "/Users/rs/workspace/cncsvision/experiments/texturing/floor_noise_vertex_shader.vert";
char fragmentShaderFloorFile[] = "/Users/rs/workspace/cncsvision/experiments/texturing/floor_noise_fragment_shader.frag";
#endif

#ifdef __linux__
char vertexShaderFile[] = "/home/carlo/workspace/cncsvisioncmake/experiments/texturing/vertex_perlin.vert";
char fragmentShaderFile[] = "/home/carlo/workspace/cncsvisioncmake/experiments/texturing/fragment_perlin.frag";
char vertexShaderFloorFile[] = "/home/carlo/workspace/cncsvisioncmake/experiments/texturing/floor_noise_vertex_shader.vert";
char fragmentShaderFloorFile[] = "/home/carlo/workspace/cncsvisioncmake/experiments/texturing/floor_noise_fragment_shader.frag";
#endif
glsl::glShader *floorStimulusShader;

GLuint fbo;
GLuint fboDepth;
GLuint fboTexture;


//=================================//
//           GLFW callbacks        //
//=================================//
/**
 * @brief error_callback
 * @param error
 * @param description
 */
static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

/**
 * @brief key_callback
 * @param window
 * @param key
 * @param scancode
 * @param action
 * @param mods
 */
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //if (action==GLFW_PRESS || action==GLFW_ACCUM_ALPHA_BITS)
    {
        switch (key)
        {
        case GLFW_KEY_ENTER:
        {
            surface.potatoSurface.seed = mathcommon::unifRand();
            surface.fillVolumeWithSpheres(500,TEXTURE_SIZE/30,TEXTURE_SIZE/10);
            break;
        }
        case GLFW_KEY_ESCAPE:
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        }
        }
    }
}

/**
 * @brief mouse_button_callback
 * @param window
 * @param button
 * @param action
 * @param mods
 */
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xpos,ypos;
        glfwGetCursorPos(window,&xpos,&ypos);
        arcball.startRotation(SCREEN_WIDTH-xpos,ypos);
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        arcball.stopRotation();

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action==GLFW_PRESS)
        arcball.reset();
}

/**
 * @brief mouse_entered_callback
 * @param window
 * @param entered
 */
static int cursor_inside=false;
static void cursor_enter_callback(GLFWwindow *window, int entered)
{
    cursor_inside=entered;
}

/**
 * @brief scroll_callback
 * @param window
 * @param xoffset
 * @param yoffset
 */
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (yoffset > 0 )
        eyeDistance += 10;
    else
        eyeDistance -= 10;
}

/**
 * @brief mouse_cursor_callback
 * @param window
 * @param xpos
 * @param ypos
 */
static void mouse_cursor_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (cursor_inside)
    {
        arcball.updateRotation(SCREEN_WIDTH-xpos,ypos);
    }
}


/**
 * @brief initOpenGLContext
 * @param width
 * @param height
 * @param refreshRate
 * @param gamemode
 * @param stereo
 * @param fsaaSamples
 * @return
 */
GLFWwindow *initOpenGLContext(const int width=1024, const int height=768,
                              const int refreshRate=60,
                              const bool gamemode=false,
                              const bool stereo=false,
                              const int fsaaSamples=1)
{
    if (!glfwInit())
    {
        throw std::runtime_error("[GLFW] Can't initialize a valid GLFW context here");
        exit(EXIT_FAILURE);
    }
    GLFWwindow* window;
    int widthMM=0;
    int heightMM=0;
    int sizeX=0;
    int sizeY=0;

    // http://www.glfw.org/docs/latest/window.html#window_hints
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    glfwGetMonitorPhysicalSize(monitor, &widthMM, &heightMM);
    glfwWindowHint(GLFW_REFRESH_RATE,refreshRate);
    glfwWindowHint(GLFW_RED_BITS,8);
    glfwWindowHint(GLFW_GREEN_BITS,8);
    glfwWindowHint(GLFW_BLUE_BITS,8);
    glfwWindowHint(GLFW_ALPHA_BITS,8);
    glfwWindowHint(GLFW_SAMPLES,fsaaSamples);
    if (stereo)
        glfwWindowHint(GLFW_STEREO,stereo);
    if (gamemode)
        window = glfwCreateWindow(width,height, "GLFW", monitor, NULL);
    else
        window = glfwCreateWindow(width,height, "GLFW", NULL, NULL);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    glfwMakeContextCurrent(window);
    glfwGetWindowSize(window,&sizeX,&sizeY);

    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Set GLFW error callback
    glfwSetErrorCallback(error_callback);
    // Set keyboard event callback
    glfwSetKeyCallback(window, key_callback);
    // Set mouse button pressed callback
    glfwSetMouseButtonCallback(window,mouse_button_callback);
    // Set mouse cursor entered callback
    glfwSetCursorEnterCallback(window,cursor_enter_callback);
    // Set mouse scroll callback
    glfwSetScrollCallback(window,mouse_scroll_callback);
    // Set mouse cursor callback
    glfwSetCursorPosCallback(window,mouse_cursor_callback);

    // Set swap interval to 1 frame
    glfwSwapInterval(1);
#ifdef DEBUG
    int count=-1;
    const GLFWvidmode *currentMode= glfwGetVideoMode(monitor);
    const GLFWvidmode *allModes = glfwGetVideoModes(monitor, &count);
    cerr << "[GLFW] Current video mode [" << currentMode->width << "x" << currentMode->height << "]" << currentMode->redBits << " " << currentMode->greenBits << " " << currentMode->blueBits << endl;
    cerr << "[GLFW] Detected monitor size [" << widthMM  << "x" << "]" << heightMM << endl;
    cerr << "[GLFW] All possible video modes: " << endl;
    for(int i = 0; i < count; i++)
    {
        cerr << "\t" << allModes[i].width << "x" << allModes[i].height << " (" << allModes[i].redBits << "," << allModes[i].greenBits << "," << allModes[i].blueBits << ") @" << allModes[i].refreshRate << endl;
    }
    cerr << "[GLFW Current fullscreen window size= [" << sizeX << "x" << sizeY << "]" << endl;
#endif

#ifdef WIN32
    // Turn on vertical screen sync under Windows.
    // (I.e. it uses the WGL__swap_control extension)
    typedef BOOL (WINAPI *PFNWGLSWAPINTERVALPROC)(int interval);
    PFNWGLSWAPINTERVALPROC wglSwapInterval = NULL;
    wglSwapInterval = (PFNWGLSWAPINTERVALPROC)wglGetProcAddress("wglSwapInterval");
    if(wglSwapInterval)
        wglSwapInterval(1);
#endif
    // Disable mouse cursor
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    return window;
}

/**
 * @brief initializeGL
 * @return handle to the GLFW window
 */
GLFWwindow *initializeGL()
{
    GLFWwindow *window = initOpenGLContext(SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_REFRESH_RATE,GameMode,IsStereo,4);
    glewExperimental = true; // Needed for core profile
    if ( glewInit() != GLEW_OK)
        throw std::runtime_error("Cannot initialize GLEW");

    int argc=0;
    char *argv[]={""};
    glutInit(&argc,argv);

    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Enable blend and blend function
    glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set depth buffer clear value
    glClearDepth(1.0);
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Enable multisampling
    glEnable(GL_MULTISAMPLE);
    // Set smooth points
    glEnable( GL_POINT_SMOOTH );
    glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);

    // Set smooth lines
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
    // Set smooth polygons
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);

    // Set depth function
    glDepthFunc(GL_LEQUAL);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    return window;
}

/**
 * @brief initializeExperiment
 */
void initializeExperiment()
{
    // Initialize the camera
    Screen screen(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH,0.0,0.0,focalDistance);
    cam.init(screen);
    cam.setNearFarPlanes(50.0,1800.0);
    arcball.setWidthHeight(SCREEN_WIDTH,SCREEN_HEIGHT);

    // Initialize the potato stimulus
    surface.fillVolumeWithSpheres(500,TEXTURE_SIZE/30,TEXTURE_SIZE/10);
    surface.initializeTexture();
    surface.initializeSurfaceShaders(VolumetricSurfaceIntersection::SurfacePotato);
    surface.potatoSurface.seed = 1.0;
    surface.potatoSurface.scale = 50.0f;
    surface.potatoSurface.zScale = 1.0f;
    surface.potatoSurface.normalScale = 0.5f;

    surface.potatoSurface.ambientColor << 0.2,0.0,0.0,1.0;
    surface.potatoSurface.diffuseColor << 0.2,0.0,0.0,1.0;
    surface.potatoSurface.specularColor << 0.2,0.2,0.2;
    surface.potatoSurface.lightPosition << 1,1,1,1;
    surface.potatoSurface.shininess = 100.0f;

    // Initialize the floor stimulus
    glsl::glShaderManager SM;
    floorStimulusShader = SM.loadfromFile(vertexShaderFloorFile,fragmentShaderFloorFile);
}

/**
 * @brief drawText
 */
void drawText()
{
    GLText text(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite);
    text.enterTextInputMode();
    text.draw("SomeText = ");
    text.leaveTextInputMode();
}

/**
 * @brief drawFrameBuffer
 */
void drawFrameBuffer()
{
    glViewport(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0,0.0,0.0,1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    cam.setEye(Vector3d::Zero());
    glTranslated(0.0,0.0,focalDistance+eyeDistance);

    glPushMatrix();
    arcball.applyRotationMatrix();
    // Bind our frame buffer texture
    glBindTexture(GL_TEXTURE_2D, fboTexture);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-50.0f, -50.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-50.0f, 50.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(50.0f, 50.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(50.0f, -50.0f, 0.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind any textures
    glPopMatrix();
}

/**
 * @brief drawFloor
 */
void drawCube()
{
    floorStimulusShader->begin();
    floorStimulusShader->setUniform3f((GLcharARB*)"cubeSize",(GLfloat)350.0f,(GLfloat)200.0f,(GLfloat)800.0f);
    floorStimulusShader->setUniform1f((GLcharARB*)"textureScale",(GLfloat)25.0f);

    glPushMatrix();
    // top
    glBegin(GL_QUADS);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glEnd();

    // bottom
    glBegin(GL_QUADS);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glEnd();

    // right
    glBegin(GL_QUADS);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glEnd();

    // left
    glBegin(GL_QUADS);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glEnd();

    // bottom
    glBegin(GL_QUADS);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glEnd();

    // back
    glBegin(GL_QUADS);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glEnd();

    glPopMatrix();

    floorStimulusShader->end();
}

/**
 * @brief drawTrial
 */
void drawTrial()
{
    glPushMatrix();
    arcball.applyRotationMatrix();
    //drawCube();
    surface.draw();
    glPopMatrix();
    //drawText();
}

/**
 * @brief initFrameBufferDepthBuffer
 */
void initFrameBufferDepthBuffer(void)
{
    // Generate one render buffer and store the ID in fbo_depth
    glGenRenderbuffers(1, &fboDepth);
    // Bind the fbo_depth render buffer
    glBindRenderbuffer(GL_RENDERBUFFER, fboDepth);
    // Set the render buffer storage to be a depth component, with a width and height of the window
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT);
    // Set the render buffer of this buffer to the depth buffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fboDepth); // XXX GENERA UN ERRORE
    // Unbind the render buffer
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

/**
 * @brief initFrameBufferTexture
 */
void initFrameBufferTexture(void)
{
    glGenTextures(1, &fboTexture); // Generate one texture
    glBindTexture(GL_TEXTURE_2D, fboTexture); // Bind the texture fbo_texture
    // Create a standard texture with the width and height of our window
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    // Setup the basic texture parameters
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

/**
 * @brief initFrameBuffer
 */
void initFrameBuffer(void)
{
    glEnable(GL_TEXTURE_2D); // Enable texturing so we can bind our frame buffer texture
    glEnable(GL_DEPTH_TEST); // Enable depth testing
    glDepthFunc(GL_LEQUAL);
    initFrameBufferDepthBuffer(); // Initialize our frame buffer depth buffer
    initFrameBufferTexture(); // Initialize our frame buffer texture
    // Generate one frame buffer and store the ID in fbo
    glGenFramebuffers(1, &fbo);
    // Bind our frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    // Attach the texture fbo_texture to the color buffer in our frame buffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture, 0);
    // Attach the depth buffer fbo_depth to our frame buffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fboDepth);
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER); // Check that status of our generated frame buffer
    if ( status != GL_FRAMEBUFFER_COMPLETE ) // If the frame buffer does not report back as complete
    {
        throw std::runtime_error("Couldn't create frame buffer"); // Output an error to the console
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind our frame buffer
}

/**
 * @brief drawGLScene
 */
void drawGLScene()
{
    // Bind a framebuffer object
    //glBindFramebuffer(GL_FRAMEBUFFER, fbo); // Bind our frame buffer for rendering
    // Binocular stereo
    if (IsStereo)
    {
        glDrawBuffer(GL_BACK);
        // Draw left eye view
        glDrawBuffer(GL_BACK_LEFT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        cam.setEye(Vector3d(interocularDistance/2,0,0));
        glTranslated(0,0,focalDistance+eyeDistance);
        drawTrial();
        // Draw right eye view
        glDrawBuffer(GL_BACK_RIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        cam.setEye(Vector3d(-interocularDistance/2,0,0));
        glTranslated(0,0,focalDistance);
        drawTrial();
    }
    else // MONOCULAR
    {
        glViewport(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        cam.setEye(Vector3d::Zero());
        glTranslated(0.0,0.0,focalDistance+eyeDistance);
        drawTrial();
    }
    // get back to the default framebuffer
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //drawFrameBuffer();
}

/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
    // http://r3dux.org/2011/05/simple-opengl-fbo-textures/
    // http://www.lighthouse3d.com/tutorials/opengl-short-tutorials/opengl_framebuffer_objects/
    GLFWwindow *window = initializeGL();
    initFrameBuffer(); // Create our frame buffer object
    initializeExperiment();
    while ( !glfwWindowShouldClose(window) )
    {
        drawGLScene();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}
