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

#ifndef _INCLUDE_CNCSVISION_GL
#define _INCLUDE_CNCSVISION_GL

// This is to define GLEW conditionally if there are already some libraries that have include OpenGL
// like Qt or others

#if !defined(__gl_h_) && !defined(__GL_H__) && !defined(__X_GL_H)
#include <GL/glew.h>
#endif

#ifdef __linux__
#include <GL/glut.h>
#endif

#if defined _WIN32 || defined WIN32
//#include <windows.h>
#include "glut.h"
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif

#endif
