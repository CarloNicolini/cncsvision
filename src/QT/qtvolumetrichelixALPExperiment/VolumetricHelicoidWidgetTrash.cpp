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

// Repository of old unused methods for VolumetricHelicoidWidget
    void drawVisualizedCameraScene();
void VolumetricHelicoidWidget::drawVisualizedCameraScene()
{
    glPushMatrix();
    glLoadIdentity();
    glTranslated(0.0,0.0,eyeZ);
    arcball.applyRotationMatrix();
    glColor3d(1.0,1.0,1.0);

    if ( isDrawingFrustum )
    {
        Eigen::Matrix4d P,MV,iMV;
        glGetDoublev(GL_MODELVIEW_MATRIX,MV.data());
        glGetDoublev(GL_PROJECTION_MATRIX,P.data());
        iMV = MV.inverse();
        drawFrustum(P.data(),iMV.data());
    }
    else
    {
        drawFrustum(cam->getOpenGLProjectionMatrix().data(),&cam->getOpenGLModelViewMatrix().inverse().matrix()(0,0));
    }
    glPopMatrix();


    // Draw the world reference frame
    glPushMatrix();
    glLoadIdentity();
    glTranslated(0,0,eyeZ);
    arcball.applyRotationMatrix();
    glBegin(GL_LINES);
    glColor3d(1,0,0);
    glVertex3d(0,0,0);
    glVertex3d(50,0,0);

    glColor3d(0,1,0);
    glVertex3d(0,0,0);
    glVertex3d(0,50,0);

    glColor3d(0,0,1);
    glVertex3d(0,0,0);
    glVertex3d(0,0,50);
    glEnd();
    glPopMatrix();


    glPushMatrix();
    glLoadIdentity();
    glTranslated(0,0,this->eyeZ);
    arcball.applyRotationMatrix();
    glRotated(90,1,0,0);
    volume->draw();
    glPopMatrix();

    glPushMatrix();
    glLoadIdentity();
    glTranslated(0,0,this->eyeZ);
    arcball.applyRotationMatrix();
    glTranslated(volume->meshStruct.offsetX,volume->meshStruct.offsetY,volume->meshStruct.offsetZ);
    if (!isSavingFrames)
        glutWireCube(volume->meshStruct.radius*2);
    glPopMatrix();
}

