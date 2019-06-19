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
#include "LatestCalibration.h"
#include "RoveretoMotorFunctions.h"

#include "beep.h"
#include "GLUtils.h"
#include "VRCamera.h"
#include "GLText.h"
#include "VolumetricSurfaceIntersection.h"

#include "CoordinatesExtractor2.h"

#include "ParametersLoader.h"
#include "BalanceFactor.h"
#include "Optotrak2.h"

using std::cerr;
using std::endl;
using std::string;

#ifdef __linux__
#undef SCREEN_WIDTH
#define SCREEN_WIDTH 1680
#undef SCREEN_HEIGHT
#define SCREEN_HEIGHT 1050
#endif

//=================================//
//    OPENGL Graphics Variables    //
//=================================//
// This variables affect the experiment from the beginning
bool GameMode=false;
bool IsStereo=false;
bool IsPictorialCondition=false;

//=================================//
// EXPERIMENTS FUNCTION PROTOTYPES //
//=================================//
void drawTrial();
void drawGL();
void advanceTrial();
void udpateStimulus();
void changeFocalDistance(double);

// Camera and visualization setup
double homeFocalDistance=-418.5;
double interocularDistance=65;
VRCamera cam;

//=================================//
//    Optotrak variable            //
//=================================//
Optotrak2 optotrak;
std::vector<Marker> markers;
bool headCalibrationKeyPressed=false;
bool fingersCalibrationKeyPressed=false;

//================================//
//     EXPERIMENTAL VARIABLES     //
//================================//
enum EXPERIMENTAL_PHASE
{
    PHASE_CALIBRATION_FINGERS=0,
    PHASE_CALIBRATION_HEAD=1,
    PHASE_EXPERIMENT = 2,
    PHASE_EXPERIMENT_FINISHED = 3
} experimentalPhase;

enum TRIAL_CONDITION
{
    TRIAL_CONDITION_PERCEPTION = 0,
    TRIAL_CONDITION_REACHING = 1
} trialCondition;

enum REACHING_TRIAL_PHASE
{
    REACHING_FIXATION_PHASE = 0,
    REACHING_STIMULUS_PHASE = 1
} reachingTrialPhase;

enum PERCEPTION_TRIAL_PHASE
{
    PERCEPTION_FIXATION_PHASE = 0,
    PERCEPTION_STIMULUS_PHASE = 1,
    PERCEPTION_ADJUSTMENT_PHASE = 2,
} perceptionTrialPhase;

enum HEAD_CALIBRATION
{
    HEAD_CALIBRATION_REGISTER_EYES=0,
    HEAD_CALIBRATION_HEAD_ALIGNMENT=1,
    HEAD_CALIBRATION_OK=2
} headCalibration;

enum FINGERS_CALIBRATION
{
    FINGERS_CALIBRATION_PLATFORM_POSITION=0,
    FINGERS_CALIBRATION_FINGERS_ON_PLATFORM=1,
    FINGERS_CALIBRATION_OK=2
} fingersCalibration;

double calibratedZScale=0.0;

Eigen::Vector3d platformThumb,platformIndex;
CoordinatesExtractor2 headCoords,thumbCoords,indexCoords;

ParametersLoader params;
BalanceFactor<double> trialFactors;

//=================================//
//  EXPERIMENTAL VISUAL STIMULI    //
//=================================//
VolumetricSurfaceIntersection potatoSurface;
glsl::glShader *surroundingCubeShader;

GLuint fbo;
GLuint fboDepth;
GLuint fboTexture;
//#define USE_FRAMEBUFFER 1

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
    if (action==GLFW_REPEAT)
    {
        switch (key)
        {
        case GLFW_KEY_PAGE_UP:
        {
            potatoSurface.potatoSurface.zScale += 0.1;
            break;
        }
        case GLFW_KEY_PAGE_DOWN:
        {
            potatoSurface.potatoSurface.zScale -= 0.1;
            break;
        }
        }
    }
    if (action==GLFW_PRESS) // To avoid multiple key press
    {
        switch (key)
        {
        case GLFW_KEY_ESCAPE:
        case GLFW_KEY_Q:
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        }
        case GLFW_KEY_SPACE:
        {
            if (fingersCalibration!=FINGERS_CALIBRATION_OK && headCalibration!=HEAD_CALIBRATION_OK)
                return;
            switch(experimentalPhase)
            {
            case PHASE_CALIBRATION_FINGERS:
            {
                experimentalPhase=PHASE_CALIBRATION_HEAD;
                break;
            }
            case PHASE_CALIBRATION_HEAD:
            {
                experimentalPhase=PHASE_EXPERIMENT;
                break;
            }
                break;
            }
            break;
        }
        case GLFW_KEY_F:
        {
            fingersCalibrationKeyPressed=true;
            break;
        }
        case GLFW_KEY_H:
        {
            headCalibrationKeyPressed=true;
            break;
        }
        case GLFW_KEY_A:
        {
            // Advance trial
            if ( !trialFactors.next() )
            {
                experimentalPhase = PHASE_EXPERIMENT_FINISHED;
            }
            changeFocalDistance(trialFactors.getCurrent().at("FocalDistance"));
            udpateStimulus();
            break;
        }
        case GLFW_KEY_B:
        {
            switch (perceptionTrialPhase)
            {
            case PERCEPTION_FIXATION_PHASE:
            {
                perceptionTrialPhase = PERCEPTION_STIMULUS_PHASE;
                break;
            }
            case PERCEPTION_STIMULUS_PHASE:
            {
                perceptionTrialPhase = PERCEPTION_ADJUSTMENT_PHASE;
                break;
            }
            case PERCEPTION_ADJUSTMENT_PHASE:
            {
                perceptionTrialPhase = PERCEPTION_FIXATION_PHASE;
                break;
            }
                break;
            }

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
    if (action == GLFW_PRESS)
    {
        switch (button)
        {
        case GLFW_MOUSE_BUTTON_1:
        {
            if (perceptionTrialPhase == PERCEPTION_STIMULUS_PHASE)
                perceptionTrialPhase = PERCEPTION_ADJUSTMENT_PHASE;
            else if (perceptionTrialPhase == PERCEPTION_ADJUSTMENT_PHASE)
                perceptionTrialPhase = PERCEPTION_STIMULUS_PHASE;
            break;
        }
        case GLFW_MOUSE_BUTTON_2:
        {

            break;
        }
        }
    }
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
    if (perceptionTrialPhase == PERCEPTION_ADJUSTMENT_PHASE)
        calibratedZScale += yoffset*0.01;
}

/**
 * @brief mouse_cursor_callback
 * @param window
 * @param xpos
 * @param ypos
 */
static void mouse_cursor_callback(GLFWwindow *window, double xpos, double ypos)
{

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


    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
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
    GLFWwindow *window = initOpenGLContext(SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_REFRESH_RATE,GameMode,IsStereo,1);
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
    //glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST); // RENDE VISIBILI LE LINEE NERE SULLO STIMOLO, non va bene
    // Set depth function
    glDepthFunc(GL_LEQUAL);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    return window;
}

/**
 * @brief changeFocalDistance
 * @param newFocalDistance
 */
void changeFocalDistance(double newFocalDistance)
{
    //RoveretoMotorFunctions::moveScreenAbsolute(newFocalDistance,focalDistance,3500);
    bool useMirror = true;
    cam.init(Screen(SCREEN_WIDE_SIZE, SCREEN_WIDE_SIZE*SCREEN_HEIGHT/SCREEN_WIDTH,0.0,0.0,newFocalDistance), useMirror);
}

/**
 * @brief udpateStimulus
 */
void udpateStimulus()
{
    int texSize = static_cast<int>(params.get("PotatoVolumeTextureSizeVoxels"));
    int nSpheres = static_cast<int>(params.get("PotatoNumberSpheres"));

    // Initialize the potato stimulus
    potatoSurface.fillVolumeWithSpheres(nSpheres,static_cast<int>(params.get("PotatoSpheresMinRadius")),static_cast<int>(params.get("PotatoSpheresMaxRadius")));

    potatoSurface.potatoSurface.seed = trialFactors.getCurrent().at("PotatoShape");
    potatoSurface.potatoSurface.scale = 50.0;
    potatoSurface.potatoSurface.zScale = trialFactors.getCurrent().at("PotatoZScale");
    potatoSurface.potatoSurface.normalScale = 0.5f;

    potatoSurface.potatoSurface.ambientColor << 0.2,0.0,0.0,1.0;
    potatoSurface.potatoSurface.diffuseColor << 0.2,0.0,0.0,1.0;
    potatoSurface.potatoSurface.specularColor << 0.2,0.2,0.2;
    potatoSurface.potatoSurface.lightPosition << 1,1,1,1;
    potatoSurface.potatoSurface.shininess = 100.0f;
}

/**
 * @brief initializeExperiment
 */
void initializeExperiment()
{
    // Initialize the experimental variables, the trial factors etc
    //params.reset();
    //params.loadParameterFile("synopter_params.txt");
    trialFactors.init(params);
    trialFactors.next();

    // Move everything to home
    // RoveretoMotorFunctions::homeObject(3500);
    // RoveretoMotorFunctions::homeScreen(3500);
    // RoveretoMotorFunctions::homeMirror(3500);

    // Initialize the camera and interocular distance
    changeFocalDistance( trialFactors.getCurrent().at("FocalDistance") );
    //cam.setNearFarPlanes(50.0,1800.0);
    interocularDistance = params.get("IOD");

    int texSize = static_cast<int>(params.get("PotatoVolumeTextureSizeVoxels"));
    potatoSurface.resize(texSize,texSize,texSize);

    udpateStimulus();
    // Necessary calls but only 1 time
    potatoSurface.initializeTexture();
    potatoSurface.initializeSurfaceShaders(VolumetricSurfaceIntersection::SurfacePotato);
    // Initialize the floor stimulus
    glsl::glShaderManager SM;
    char vertexShaderFloorFile[] = "../experiments/texturing/floor_noise.vert";
    char fragmentShaderFloorFile[] = "../experiments/texturing/floor_noise.frag";
    surroundingCubeShader = SM.loadfromFile(vertexShaderFloorFile,fragmentShaderFloorFile);

    // Set the adjustment z scale of the potato
    calibratedZScale = trialFactors.getCurrent().at("PotatoZScale");
}


/**
 * @brief drawText
 */
void drawText(const std::string &infotext)
{
    GLText text(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite,GLUT_BITMAP_TIMES_ROMAN_24);
    text.enterTextInputMode();
    text.draw(infotext);
    text.leaveTextInputMode();
}

/**
 * @brief drawFrameBuffer
 */
void drawFrameBuffer()
{
    GLfloat textureFrameSize=SCREEN_WIDE_SIZE/2;
    GLfloat ratio = static_cast<GLfloat>(SCREEN_HEIGHT)/static_cast<GLfloat>(SCREEN_WIDTH);
    glViewport(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0,0.0,0.0,1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    cam.setEye(Vector3d::Zero());
    glTranslated(0.0,0.0,homeFocalDistance);
    glPushMatrix();
    // Bind our frame buffer texture
    glBindTexture(GL_TEXTURE_2D, fboTexture);
    static double x=0;
    x+=0.1;
    double z=sin(x)*10-418.5;
    z=0;
    // Must use right texcoords because framebuffer has the origin in lower left,
    // monitor has origin in upper left
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(textureFrameSize, -textureFrameSize*ratio, z);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-textureFrameSize, -textureFrameSize*ratio, z);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-textureFrameSize, textureFrameSize*ratio, z);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(textureFrameSize, textureFrameSize*ratio, z);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind any textures
    glPopMatrix();
}

/**
 * @brief drawCube
 */
void drawCube()
{
    surroundingCubeShader->begin();
    surroundingCubeShader->setUniform3f((GLcharARB*)"cubeSize",(GLfloat)200.0f,(GLfloat)200.0f,(GLfloat)200.0f);
    surroundingCubeShader->setUniform1f((GLcharARB*)"textureScale",(GLfloat)25.0f);

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

    surroundingCubeShader->end();
}

/**
 * @brief drawCalibrationSemaphore
 * @param displayText
 * @param proceed
 * @return
 */
bool drawCalibrationSemaphore(const std::string& displayText, bool proceed)
{
    GLText text(SCREEN_WIDTH,SCREEN_HEIGHT,glWhite);
    text.enterTextInputMode();
    text.draw(displayText);
    text.leaveTextInputMode();
    // Draw a semaphore
    glPushMatrix();
    if ( proceed )
        glColor3fv(glGreen);
    else
        glColor3fv(glRed);
    glScaled(1,1,0.001);
    glutSolidSphere(20,40,40);
    glPopMatrix();
    return proceed;
}

/**
 * @brief drawStimulusFront
 */
void drawStimulusFront()
{
    potatoSurface.potatoSurface.ambientColor << 0.2,0.0,0.0,1.0;
    glPushMatrix();
    drawCube();
    potatoSurface.potatoSurface.zScale = trialFactors.getCurrent().at("PotatoZScale");
    //potatoSurface.draw();
    //glRotated(30,0,1,0);
    glutSolidTeapot(50);
    glutWireCube(50);
    glPopMatrix();
}

/**
 * @brief drawFixation
 */
void drawFixation()
{
    glPushAttrib(GL_POINT_BIT);
    glPointSize(5);
    glBegin(GL_POINTS);
    glVertex3d(150,0,homeFocalDistance);
    glEnd();
    glPopAttrib();
}

/**
 * @brief drawAdjustmentCrossSection
 */
void drawAdjustmentCrossSection()
{
    potatoSurface.potatoSurface.zScale = calibratedZScale;
    potatoSurface.potatoSurface.ambientColor << 1.0,1.0,1.0,1.0;
    GLfloat clipWidth = 0.5f;
    GLfloat eq0[] = {0.0f, 1.0f, 0.0f, 0.5f};
    GLfloat eq1[] = {0.0f, -1.0f, 0.0f, -0.5f};
    glPushAttrib(GL_POLYGON_BIT | GL_LIGHTING_BIT | GL_POINT_BIT | GL_LINE_BIT);
    glLineWidth(5);
    glPointSize(5);
    glPushMatrix();
    glRotated(90,1,0,0);
    glEnable(GL_CLIP_PLANE0);
    glClipPlane(GL_CLIP_PLANE0,eq0);
    glEnable(GL_CLIP_PLANE1);
    glClipPlane(GL_CLIP_PLANE1,eq1);
    //potatoSurface.draw();
    //glRotated(30,0,1,0);
    //glutSolidTeapot(50);
    glutWireCube(50);
    glDisable(GL_CLIP_PLANE1);
    glDisable(GL_CLIP_PLANE0);
    glPopMatrix();
    glPopAttrib();
}

/**
 * @brief drawPerceptionTrial
 */
void drawPerceptionTrial()
{
    switch (perceptionTrialPhase)
    {
    case PERCEPTION_FIXATION_PHASE:
    {
        drawFixation();
        break;
    }
    case PERCEPTION_STIMULUS_PHASE:
    {
        drawStimulusFront();
        break;
    }
    case PERCEPTION_ADJUSTMENT_PHASE:
    {
        drawAdjustmentCrossSection();
        break;
    }
    }
}

/**
 * @brief drawReachingTrial
 */
void drawReachingTrial()
{
    switch (reachingTrialPhase)
    {
    case REACHING_FIXATION_PHASE:
    {
        break;
    }
    case REACHING_STIMULUS_PHASE:
    {
        break;
    }
    }
}

/**
 * @brief drawTrial
 */
void drawTrial()
{
    switch (trialCondition)
    {
    case TRIAL_CONDITION_PERCEPTION:
    {
        drawPerceptionTrial();
        break;
    }
    case TRIAL_CONDITION_REACHING:
    {
        drawReachingTrial();
        break;
    }
    }
}

/**
 * @brief initFrameBufferDepthBuffer
 */
void initializeFrameBufferDepthBuffer(void)
{
    // Generate one render buffer and store the ID in fbo_depth
    glGenRenderbuffers(1, &fboDepth);
    // Bind the fbo_depth render buffer
    glBindRenderbuffer(GL_RENDERBUFFER, fboDepth);
    // Set the render buffer storage to be a depth component, with a width and height of the window
    glRenderbufferStorage(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT);
    // Set the render buffer of this buffer to the depth buffer
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fboDepth); // XXX GENERA UN ERRORE
    // Unbind the render buffer
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

/**
 * @brief initFrameBufferTexture
 */
void initializeFrameBufferTexture(void)
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
void initializeFrameBuffer(void)
{
    glEnable(GL_TEXTURE_2D); // Enable texturing so we can bind our frame buffer texture
    //glEnable(GL_DEPTH_TEST); // Enable depth testing
    //glDepthFunc(GL_LEQUAL);

    initializeFrameBufferDepthBuffer(); // Initialize our frame buffer depth buffer
    initializeFrameBufferTexture(); // Initialize our frame buffer texture
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
    getGLerrors();
}


/**
 * @brief drawScene
 */
void drawScene()
{
    switch (experimentalPhase)
    {
    case PHASE_CALIBRATION_FINGERS:
    {
        switch (fingersCalibration)
        {
        case FINGERS_CALIBRATION_PLATFORM_POSITION:
        {
            if ( drawCalibrationSemaphore(string("Keep the platform markers 15 and 16 BOTH visible, then press F..."),isVisible(markers.at(15).p) && isVisible(markers.at(16).p)) && fingersCalibrationKeyPressed )
            {
                platformThumb = markers.at(15).p;
                platformIndex = markers.at(16).p;
                fingersCalibration = FINGERS_CALIBRATION_FINGERS_ON_PLATFORM;
                Beep(440,440);
            }
            break;
        }
        case FINGERS_CALIBRATION_FINGERS_ON_PLATFORM:
        {
            Vector3d indexCentroid = (markers.at(7).p+markers.at(8).p+markers.at(9).p)/3.0;
            Vector3d thumbCentroid = (markers.at(11).p+markers.at(12).p+markers.at(13).p)/3.0;
            if ( drawCalibrationSemaphore(string("Keep the thumb and index on markers 15 and 16 BOTH visible, then press F..."),isVisible(indexCentroid) && isVisible(thumbCentroid )) && fingersCalibrationKeyPressed)
            {
                indexCoords.init(platformIndex, markers.at(7).p, markers.at(8).p, markers.at(9).p);
                thumbCoords.init(platformThumb, markers.at(11).p, markers.at(12).p, markers.at(13).p);
                fingersCalibration = FINGERS_CALIBRATION_OK;
                // Proceed to head calibration
                experimentalPhase = PHASE_CALIBRATION_HEAD;
                Beep(440,440);
            }
            break;
        }
        }
        fingersCalibrationKeyPressed=false; // Reset the status of key press
        break;
    }
    case PHASE_CALIBRATION_HEAD:
    {
        switch (headCalibration)
        {
        case HEAD_CALIBRATION_REGISTER_EYES:
        {
            if ( drawCalibrationSemaphore(string("Register the MASK left and right eye then press H..."),isVisible(markers.at(17).p) && isVisible(markers.at(18).p)) && headCalibrationKeyPressed )
            {
                headCoords.init(markers.at(17).p,markers.at(18).p, markers.at(1).p,markers.at(2).p,markers.at(3).p,interocularDistance );
                headCalibration = HEAD_CALIBRATION_HEAD_ALIGNMENT;
                Beep(440,440);
            }
            break;
        }
        case HEAD_CALIBRATION_HEAD_ALIGNMENT:
        {
            Vector3d leftEye = headCoords.getLeftEye().p;
            Vector3d rightEye = headCoords.getRightEye().p;
            if ( drawCalibrationSemaphore(string("Move the head on CHIN REST then press H..."),isVisible(leftEye) && isVisible(rightEye)) && headCalibrationKeyPressed)
            {
                headCoords.init(leftEye,rightEye,markers.at(1).p,markers.at(2).p,markers.at(3).p,interocularDistance );
                headCalibration = HEAD_CALIBRATION_OK;
                experimentalPhase=PHASE_EXPERIMENT;
                Beep(440,440);
            }
            break;
        }
        }
        headCalibrationKeyPressed=false; // Reset the status of key press
        break;
    }
    case PHASE_EXPERIMENT:
    {
        drawTrial();
        break;
    }
    }
}

/**
 * @brief drawGLScene
 */
void drawGL()
{
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
        //cam.setEye(headCoords.getRightEye().p);
        glTranslated(0,0,homeFocalDistance);
        drawScene();
        // Draw right eye view
        glDrawBuffer(GL_BACK_RIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        cam.setEye(Vector3d(-interocularDistance/2,0,0));
        //cam.setEye(headCoords.getLeftEye().p);
        glTranslated(0,0,homeFocalDistance);
        drawScene();
    }
    else // MONOCULAR
    {
        glViewport(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0,0.0,0.0,1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        cam.setEye(Vector3d::Zero());
        //cam.setEye(headCoords.getRightEye().p);
        glTranslated(0.0,0.0,homeFocalDistance);
        drawScene();
    }
}

/**
 * @brief updateOptotrak
 */
void updateOptotrak()
{
    optotrak.updateMarkers();
    markers = optotrak.getAllMarkers();
    headCoords.update(markers.at(1).p,markers.at(2).p,markers.at(3).p,10);
    // update thumb coordinates
    thumbCoords.update(markers.at(11).p,markers.at(12).p,markers.at(13).p,10);
    // update index coordinates
    indexCoords.update(markers.at(7).p, markers.at(8).p, markers.at(9).p,10);
}

/**
 * @brief readExperimentalSettings
 * @param filename
 */
void readExperimentalSettings(const std::string &filename)
{
    params.loadParameterFile(filename);

    GameMode = static_cast<bool>(params.get("GameMode"));

    if (params.find("ViewingCondition") == "MonocularAperture" || params.find("ViewingCondition") == "BinocularAperture" )
    {
        IsPictorialCondition = true;
        IsStereo = false;
        //GameMode = false;
        GameMode = true;
    }

    if (params.find("ViewingCondition") == "Synoptic" || params.find("ViewingCondition") == "Stereoscopic" )
    {
        IsPictorialCondition = false;
        // Must force StereoMode and GameMode
        GameMode = true;
        //IsStereo = true;
    }
}

/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
    readExperimentalSettings("synopter_parameters.txt");

    // Open a window with a valid OpenGL context
    GLFWwindow *window = initializeGL();
    if ( IsPictorialCondition)
        initializeFrameBuffer(); // Create our frame buffer object

    initializeExperiment();

#ifdef USE_OPTOTRAK
    optotrak.setTranslation(calibration);
    optotrak.init(LastAlignedFile,20,100);
#endif
    experimentalPhase = PHASE_EXPERIMENT;
    //IsPictorialCondition = false;
    while (!glfwWindowShouldClose(window) )
    {
#ifdef USE_OPTOTRAK
        updateOptotrak();
#endif
        if (IsPictorialCondition)
        {
            // Bind a framebuffer object
            glBindFramebuffer(GL_FRAMEBUFFER, fbo); // Bind our frame buffer for rendering
        }
        drawGL();
        // get back to the default framebuffer
        if (IsPictorialCondition)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            drawFrameBuffer();
        }
        if (experimentalPhase == PHASE_EXPERIMENT_FINISHED)
            drawText("EXPERIMENT FINISHED, PRESS ESC TO QUIT");
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}
