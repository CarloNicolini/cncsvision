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

#include "Grid.h"
#include "Util.h"
#include "Mathcommon.h"
#include "CalibrationHelper.h"
#include "GLText.h"

using namespace std;
using namespace Eigen;
using namespace mathcommon;
using util::stringify;
/**
 * \ingroup GLVisualization
 * Default constructor. Width, height and focal plane distance are set to 100. Other controls are set to false
**/
CalibrationHelper::CalibrationHelper()
{  width=height=focalPlaneDistance=100;
   printInstructions=printCalibration=printGrid=printBoxes=printFPS=printHeadCalibration=false;
}
/**
 * \ingroup GLVisualization
 * Set the width, height and depth (focal distance) of the projection plane. This must be the same as the real screen, otherwise the calibration isn't corret
 * @param _width Width of the projection plane rectangle measured in millimiter.
 * @param _height Height of the projection plane rectangle measured in millimiter
 * @param _focalPlaneDistance Distance of the projection plane (focal plane)
 **/
void CalibrationHelper::setWidthHeightFocalPlane(int _width, int _height, double _focalPlaneDistance)
{  width=_width;
   height=_height;
   focalPlaneDistance = _focalPlaneDistance;
   subwidth=width/4;
   subheight=height/2;
   printInstructions=printCalibration=printGrid=printBoxes=printFPS=printHeadCalibration=false;
}

/**
 * \ingroup GLVisualization
 * Set all the internal variables of the CalibrationHelper class using directly the current \ref Screen object (to prefer w.r.t setWidthHeightFocalPlane )
 * @param _screen The current Screen object
**/
void CalibrationHelper::setScreen(const Screen &_screen)
{  alignmentX=_screen.offsetX;
   alignmentY=_screen.offsetY;
   focalPlaneDistance=_screen.focalDistance;
   pa = _screen.pa;
   pb = _screen.pb;
   pc = _screen.pc;
}

/** \ingroup GLVisualization
 * Switch the printing of instructions on/off
**/
void CalibrationHelper::switchInstructions()
{  printInstructions=!printInstructions;
}

/**
 * \ingroup GLVisualization
 * Switch the calibration on/off
**/
void CalibrationHelper::switchCalibration()
{  printCalibration=!printCalibration;
}

/** \ingroup GLVisualization
 * Switch the printing of frames per seconds on/off
**/
void CalibrationHelper::switchFPS()
{  printFPS=!printFPS;
}

/** \ingroup GLVisualization
 * Switch the print of grid on/off
**/
void CalibrationHelper::switchGrid()
{  printGrid=!printGrid;
}

/** \ingroup GLVisualization
 * Switch the printing of 5x5x5 boxes on/off
**/
void CalibrationHelper::switchBoxes()
{  printBoxes=!printBoxes;
}

void CalibrationHelper::switchHeadCalibration()
{  printHeadCalibration=!printHeadCalibration;
}

/** \ingroup GLVisualization
 * A wrapper method around all the things to draw, if one of boxes, grid or marker is true, draw it, otherwise don'nt
 * This method is intended to be used inside the  \code glutDrawFunc((*)void()) \endcode callback, for example in the \code drawGLScene() \endcode function.
 **/
bool CalibrationHelper::somethingToDraw()
{  return ( printCalibration  );
}

/** \ingroup GLVisualization
 * A wrapper method around all the things to draw, if one of boxes, grid or marker is true, draw it, otherwise don'nt
 * This method is intended to be used inside the  \code glutDrawFunc((*)void())\endcode callback, for example in the \code drawGLScene() \endcode function.
 **/
void CalibrationHelper::draw()
{  if ( printCalibration )
   {  glDisable(GL_COLOR_MATERIAL);
      glDisable(GL_BLEND);
      glDisable(GL_LIGHTING);
      glDisable(GL_LIGHT0);
      glDisable(GL_LIGHT1);

      int initialPointSize;
      glGetIntegerv(GL_POINT_SIZE, &initialPointSize);


      glClearColor(0.3,0.3,0.3,1.0);
      // Draw the central blue point
      glPushMatrix();
      glColor3fv(glBlue);
      glPointSize(5);
      glBegin(GL_POINTS);
      glVertex3f(0,0,focalPlaneDistance );
      // Draw 4 points at the borders of the visible area
      glVertex3f(pa.x(),pa.y(),focalPlaneDistance );
      glVertex3f(pb.x(),pb.y(),focalPlaneDistance );
      glVertex3f(pc.x(),pc.y(),focalPlaneDistance );
      glVertex3f(pb.x(),pc.y(),focalPlaneDistance );
      glEnd();
      glPointSize(1);
      glPopMatrix();
      glColor3fv(glWhite);

      if ( printInstructions )
      {  GLText text;
         text.init(width,height,glWhite);
         text.enterTextInputMode();
         text.draw("1) Measure this grey area [Width,Height] [mm]");
         text.draw("2) Align the blue dot with the Optotrak (0,0,z)");
         text.draw("3) Use the numeric pad to center blue dot with (0,0,focalDist)");
         text.draw("4) Use PagUP PagDown to translate focal plane");
         text.draw("Press \'F1\' to close this mode");
         text.draw("Press \'F2\' to hide instructions");
         text.draw("Press \'g\' to show a 5cm edge grid");
         text.draw("Press \'b\' to show 5cm boxes");
         text.draw("AlignmentX= " + stringify<double>(alignmentX) );
         text.draw("AlignmentY= " + stringify<double>(alignmentY) );
         text.draw("FocalDistanceZ= " + stringify<double>(focalPlaneDistance) );
      }

      glColor3fv(glWhite);

      if (printGrid)
      {  Grid grid;
         grid.setRowsAndCols(10,10);
         grid.init(Vector3d(-50, -50, 0.0),Vector3d(50, -50, 0.0),Vector3d(-50, 50, 0.0),Vector3d(50, 50, 0.0));
         glPushMatrix();
         glTranslated(0,0,focalPlaneDistance);
         grid.draw();
         glPopMatrix();
      }

      int nAvanti=1,nDietro=2;
      glColor3fv(glWhite);
      if ( printBoxes )
      {  /*********  Draw white cubes cube with edge=50 millimeters *********/
         double edge=50;
         for (int i=-nAvanti ; i<nDietro; i++)
         {  glPushMatrix();
            glTranslatef(0,0.0, focalPlaneDistance + i*edge);
            glutWireCube(edge);
            glPopMatrix();
         }
      }
      glPointSize(initialPointSize);
   }
}

/** \ingroup GLVisualization
 * Draw the frames per second in the upper right part of the screen
 * @param fps Number to draw as FPS
**/
void CalibrationHelper::drawFPS(double fps)
{  if (printFPS)
   {  GLText text;
      text.init(width,height,glWhite);
      text.enterTextInputMode();
      text.draw("FPS= " + stringify(fps));
      text.leaveTextInputMode();
   }
}
