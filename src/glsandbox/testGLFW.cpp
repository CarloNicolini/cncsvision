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
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include "Timer.h"
#include <vector>

using namespace std;

static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

static void mouse_entered_callback(GLFWwindow *window, int entered)
{
    printf("%d\n",entered);
}

GLFWwindow *initOpenGLContext(const int width=1024, const int height=768, const int refreshRate=60, const bool gamemode=false, const bool stereo=false, const int fsaaSamples=1)
{
    GLFWwindow* window;
    int widthMM=0;
    int heightMM=0;
    int count=-1;
    int sizeX=0;
    int sizeY=0;

    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
    {
        throw std::runtime_error("[GLFW] Can't initialize a valid GLFW context here");
        exit(EXIT_FAILURE);
    }

    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *currentMode= glfwGetVideoMode(monitor);
    const GLFWvidmode *allModes = glfwGetVideoModes(monitor, &count);
    glfwGetMonitorPhysicalSize(monitor, &widthMM, &heightMM);
    // http://www.glfw.org/docs/latest/window.html#window_hints
    glfwWindowHint(GLFW_REFRESH_RATE,refreshRate);
    glfwWindowHint(GLFW_RED_BITS,8);
    glfwWindowHint(GLFW_GREEN_BITS,8);
    glfwWindowHint(GLFW_BLUE_BITS,8);
    glfwWindowHint(GLFW_ALPHA_BITS,8);
    glfwWindowHint(GLFW_SAMPLES,fsaaSamples);
    glfwWindowHint(GLFW_STEREO,stereo);
    window = glfwCreateWindow(width,height, "GLFW", monitor, NULL);
    glfwMakeContextCurrent(window);
    glfwGetWindowSize(window,&sizeX,&sizeY);

    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
#ifdef _DEBUG
    cerr << "[GLFW] Current video mode [" << currentMode->width << "x" << currentMode->height << "]" << currentMode->redBits << " " << currentMode->greenBits << " " << currentMode->blueBits << endl;
    cerr << "[GLFW] Detected monitor size [" << widthMM  << "x" << "]" << heightMM << endl;
    /*
    cerr << "[GLFW] All possible video modes: " << endl;
    for(int i = 0; i < count; i++)
    {
        cerr << "\t" << allModes[i].width << "x" << allModes[i].height << "@" << allModes[i].redBits << "," << allModes[i].greenBits << "," << allModes[i].blueBits << endl;
    }
    */
    cerr << "[GLFW Current fullscreen window size= [" << sizeX << "x" << sizeY << "]" << endl;
#endif
    glfwSwapInterval(1);
#ifdef _WIN32
    // Turn on vertical screen sync under Windows.
    // (I.e. it uses the WGL_EXT_swap_control extension)
    typedef BOOL (WINAPI *PFNWGLSWAPINTERVALEXTPROC)(int interval);
    PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;
    wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
    if(wglSwapIntervalEXT)
        wglSwapIntervalEXT(1);
#endif
    return window;
}


int main(void)
{
    GLFWwindow *window = initOpenGLContext(1680,1050,60);
    glewInit();
    glfwSetKeyCallback(window, key_callback);
    vector<double> times;
    Timer timer;
    timer.start();
    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(1,1,1,1);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        glMatrixMode(GL_MODELVIEW);

        glLoadIdentity();
        glRotatef((float) glfwGetTime() * 50.f, 1.0f, 0.f, 1.f);

        glBegin(GL_TRIANGLES);
        glColor3f(1.f, 0.f, 0.f);
        glVertex3f(-0.6f, -0.4f, 0.f);
        glColor3f(0.f, 1.f, 0.f);
        glVertex3f(0.6f, -0.4f, 0.f);
        glColor3f(0.f, 0.f, 1.f);
        glVertex3f(0.f, 0.6f, 0.f);
        glEnd();

        glfwSwapBuffers(window);
        glfwPollEvents();
        times.push_back(timer.getElapsedTimeInMilliSec());
        timer.start();
    }

    for (unsigned int i=0; i<times.size();i++)
        cout << times.at(i) << endl;

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}
