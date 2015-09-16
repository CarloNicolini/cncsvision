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

#include <exception>
#include "GLUtils.h"
#include "Mathcommon.h"
#include "StimulusDrawer.h"
#include "PointwiseStimulus.h"

//using namespace mathcommon;
/**
* \ingroup GLVisualization
* \brief Default constructor
*
* Set currentStimulus to NULL, gx=gy=0 and radius to 0.4
**/
StimulusDrawer::StimulusDrawer()
{   currentStimulus=NULL;
    gx=gy=0;
    radius=0.4;
    drawSpheres=true;
    drawSpecial=false;
	stimListIndex=0;
}

void StimulusDrawer::setSpheres(bool _drawSpheres )
{   drawSpheres=_drawSpheres;
}

/**
* \ingroup GLVisualization
* \brief Set the stimulus to draw. A valid pointer must be provided.
* @param s Pointer to a \ref Stimulus object
**/
void StimulusDrawer::setStimulus(Stimulus *s)
{
    try
    {   if ( s==NULL )
            throw ( std::bad_alloc() );
        else
            currentStimulus=s;
    }
    catch (std::exception const &e )
    {   cerr << e.what() << " Tried to assign NULL, are you sure what you're doing?" << endl;
    }
}

void StimulusDrawer::drawSpecialPoints()
{   drawSpecial=true;
}

/**
* \ingroup GLVisualization
* \brief Compile the OpenGL list to draw the stimulus faster.
* @param s Pointer to a stimulus object
**/
void StimulusDrawer::initList(Stimulus *s, const GLfloat color[4], GLfloat pointSize)
{
    if (s!=NULL)
        currentStimulus=s;
    else
        throw std::runtime_error("Assigning null pointer to StimulusDrawer");
    
    // if this is the first time a list is generated then generate it
    // else me must delete it and generate it again
    if (stimListIndex!=0)
	{
		glDeleteLists(stimListIndex,1);
	}
	stimListIndex = glGenLists(1);
    
    PointwiseStimulus *pstim = (PointwiseStimulus*)currentStimulus;
    if ( pstim == NULL  ) return;

    Point3D *p=NULL;
    PointsRandIterator pEnd = pstim->pointsRand.end();
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_CULL_FACE);    // a little optimization
	glNewList(stimListIndex, GL_COMPILE);
    glColor3fv(color);
    if  (drawSpheres )
    {   glShadeModel(GL_SMOOTH);
        for (PointsRandIterator iRand = pstim->pointsRand.begin(); iRand!=pEnd; ++iRand)
        {   p=(*iRand);
            glPushMatrix();
            glTranslated(p->x, p->y, p->z);
            glutSolidSphere(p->radius,10,10);
            glPopMatrix();
        }
        if ( drawSpecial )
        {   // Draw the special extremal points of the stimulus
            glColor3fv(glGreen);
            for (PointsRandIterator iRand = pstim->specialPointsRand.begin(); iRand!=pstim->specialPointsRand.end(); ++iRand)
            {   p=(*iRand);
                glPushMatrix();
                glTranslated(p->x, p->y, p->z);
                glutSolidSphere(p->radius,10,10);
                glPopMatrix();
            }
        }
    }
    else
    {   //glEnable(GL_CULL_FACE);
        glEnable(GL_POINT_SMOOTH);
        glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
        glDisable(GL_BLEND);
        glPointSize(pointSize);
        glBegin(GL_POINTS);
        //glColor3fv(glRed);
        for (PointsRandIterator iRand = pstim->pointsRand.begin(); iRand!=pEnd; ++iRand)
        {   p=(*iRand);
            glVertex3f(p->x, p->y, p->z);
        }
        // Draw the special extremal points of the stimulus
        if ( drawSpecial )
        {   glColor3fv(glGreen);
            for (PointsRandIterator iRand = pstim->specialPointsRand.begin(); iRand!=pstim->specialPointsRand.end(); ++iRand)
            {   p=(*iRand);
                glVertex3f(p->x, p->y, p->z);
            }
        }
        glEnd();
    }
    glEndList();
    glPopAttrib();
}

void StimulusDrawer::setRadius(double r)
{   
	if  ( drawSpheres )
    {   
		PointwiseStimulus *pstim = (PointwiseStimulus*)currentStimulus;
		if (pstim==NULL)
                throw (std::bad_alloc());
        else
        {   
			Point3D *p=NULL;
			for (PointsRandIterator iRand = pstim->pointsRand.begin(); iRand!=pstim->pointsRand.end(); ++iRand)
			{   
				p=(*iRand);
				p->radius=r;
			}
		}
	}
}

/**
* \ingroup GLVisualization
* \brief Draw the stimulus.
**/
void StimulusDrawer::draw()
{   glCallList(stimListIndex);
}

/**
* \ingroup GLVisualization
* \brief Destructor, dispose all the memory and clean the OpenGL list
**/
StimulusDrawer::~StimulusDrawer()
{   glDeleteLists(stimListIndex,1);
}

vector<Eigen::Vector3d> StimulusDrawer::projectStimulusPoints(const Affine3d &objectTransformation, const Affine3d &activeHeadTransformation, const VRCamera &cam, double focalDistance, const Screen &screen, const Vector3d &eyeCalibration, bool passiveMode, bool draw )
{   PointwiseStimulus *pstim = (PointwiseStimulus*)currentStimulus;

    unsigned int nspecialPoints = pstim->specialPointsRand.size();
    vector<Vector3d> stimPointsProjected(nspecialPoints);

    Projective3d P = cam.getProjectiveMatrix();
    Affine3d M = cam.getModelViewMatrix();

    for ( unsigned int i=0; i<nspecialPoints; i++ )
        stimPointsProjected[i] = ( P* (M*   ( objectTransformation*pstim->specialPointsRand.at(i)->toVec3d() )   ) .homogeneous() ).eval().hnormalized();

    //cerr << "stimdrawer " << stimPointsProjected.at(0).transpose() << endl;
    // The coordinates in the [-1,1] x [-1,1] x [-1,1] cube must be remapped in the projection area, once its rectangular size is known
    // SCREEN ALIGNMENT COORDINATES ARE

    vector<Vector3d> finalProjPoints(nspecialPoints);
    for ( unsigned int i=0; i<nspecialPoints; i++ )
    {   finalProjPoints[i].coeffRef(0) =  -(screen.offsetX + stimPointsProjected.at(i).coeffRef(0)*(  screen.screenWidth*0.5  ));
        finalProjPoints[i].coeffRef(1) =  -screen.offsetY + stimPointsProjected.at(i).coeffRef(1)*( screen.screenHeight*0.5 );
        finalProjPoints[i].coeffRef(2) =  focalDistance;
    }
    if ( draw )
    {
        glPushAttrib(GL_COLOR_BUFFER_BIT | GL_POINT_BIT );
        for ( unsigned int i=0; i<nspecialPoints; i++ )
            glPushMatrix();
        glColor3fv(glYellow);
        for ( unsigned int i=0; i<nspecialPoints; i++ )
        {
            if ( passiveMode )
                glLoadMatrixd(activeHeadTransformation.data());
            else
                glLoadIdentity();
            glTranslated(finalProjPoints.at(i).x(),finalProjPoints.at(i).y(),finalProjPoints.at(i).z());
            glutSolidSphere(1,10,10);
        }
    }
    glPopMatrix();
    glPopAttrib();

    Vector3d xycal = Vector3d(eyeCalibration.x(),eyeCalibration.y(),0);
    for ( unsigned int i=0; i<nspecialPoints; i++ )
        finalProjPoints.at(i)-=xycal;

    return finalProjPoints;
}

MatrixXd StimulusDrawer::computeOpticFlow(const vector<Vector3d> &projectStimulusPoints,double focalDistance,double deltaTInSeconds )
{
    // Now we must compute the angles alpha1, beta1, alpha2, beta2, alpha3, beta3 which are the angles
    // which the extremal points form with respect to the subject eye
    Vector3d oldAlpha, oldBeta;
	oldAlpha << alpha;
	oldBeta << beta;
    // alpha is the "pitch" angle, beta is the "yaw" angle

	alpha(0) =  ( atan2(projectStimulusPoints.at(4).x(),abs(focalDistance) ) );
	alpha(1) =  ( atan2(projectStimulusPoints.at(5).x(),abs(focalDistance) ) );
	alpha(2) =  ( atan2(projectStimulusPoints.at(8).x(),abs(focalDistance) ) );

	beta(0) =  ( atan2(projectStimulusPoints.at(4).y(),abs(focalDistance) ) );
	beta(1) =  ( atan2(projectStimulusPoints.at(5).y(),abs(focalDistance) ) );
	beta(2) =  ( atan2(projectStimulusPoints.at(8).y(),abs(focalDistance) ) );
    
	MatrixXd angVelocities(6,1);
    angVelocities(0) = alpha(0)-oldAlpha(0);
    angVelocities(1) = beta(0)-oldBeta(0);
    angVelocities(2) = alpha(1)-oldAlpha(1);
    angVelocities(3) = beta(1)-oldBeta(1);
    angVelocities(4) = alpha(2)-oldAlpha(2);
    angVelocities(5) = beta(2)-oldBeta(2);

    angVelocities /= deltaTInSeconds;

    MatrixXd coeffMatrix(6,6);
    coeffMatrix <<
                1, alpha(0),   beta(0), 0, 0, 0,
                   0, 0,    0,    1,alpha(0),beta(0),
                   1, alpha(1),   beta(1), 0, 0, 0,
                   0, 0,    0,    1,alpha(1),beta(1),
                   1, alpha(2),   beta(2), 0, 0, 0,
                   0, 0,    0,    1,alpha(2),beta(2)
                   ;
    //fullPivHouseholderQR
    MatrixXd velocities = coeffMatrix.colPivHouseholderQr().solve(angVelocities);

    return velocities;
}

Eigen::Vector3d StimulusDrawer::getProjectedStimulusCenter()
{   return project(Eigen::Vector3d(0,0,0));
}
