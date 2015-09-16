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


#include "GLUtils.h"


#include "Util.h"
#include "Mathcommon.h"
#include "Imageloader.h"
#include "Grid.h"
#include "Circle.h"
#include "Circle3D.h"

/**
 * @brief glPrintText Print text on current window, given a series of numbers as input, first sets the glOrtho2D mode, this function is slow, drawing text is an heavy task
 * @param _x x coordinate on the screen (0,0) is the upper left vertex, (width,height) is the lower right
 * @param _y y coordinate on the screen (0,0) is the upper left vertex, (width,height) is the lower right
 * @param _width
 * @param _height
 * @param text
 * @param numbers
 */
void glPrintText(int _x, int _y, int _width, int _height, const string &text, const vector<double> &numbers)
{
    int x=_x+20;
    int y=_y;
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, _width, 0.0, _height);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glColor3fv( glWhite );
    glRasterPos2f(x, y);
    int len = (int)text.length();
    for (int i = 0; i < len; i++)
    {  glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }
    if (numbers.size()!=0 )
    {  vector<string> sNumbers(numbers.size());
        for (unsigned int i=0; i<sNumbers.size(); i++)
            sNumbers[i]=util::stringify<double>(numbers[i]);
        for (unsigned  int i=0; i<sNumbers.size(); i++)
        {  y-=24;
            len=sNumbers[i].size();
            for (int j = 0; j < len; j++)
            {  glRasterPos2f(x,y);
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, sNumbers[i][j]);
                x+=10;
            }
            x=_x+20;
        }
    }
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}
/**
 * @brief glPrintText prints text on current window given a string of text
 * @param _x x coordinate on the screen (0,0) is the upper left vertex, (width,height) is the lower right
 * @param _y y coordinate on the screen (0,0) is the upper left vertex, (width,height) is the lower right
 * @param _width
 * @param _height
 * @param text text to print
 */
void glPrintText(int _x, int _y, int _width, int _height, const string &text )
{
    int x=_x+20;
    int y=_y;
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, _width, 0.0, _height);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glColor3fv( glWhite );
    glRasterPos2f(x, y);
    int len = (int)text.length();
    for (int i = 0; i < len; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

/**
 * @brief beginOrtho Begin orthographic 2D projection mode, needed for text displaying
 */
void beginOrtho()
{  glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    GLint viewport [4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    gluOrtho2D(0, viewport[2], viewport[3], 0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
}

/**
 * @brief endOrtho End orthographic display mode
 */
void endOrtho()
{  glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

/**
 * @brief circleMask Fill with black the outside area of a circle centered in circleCenterx,circleCenterY with radius circleRadius.
 * OpenGL stencil buffer must be initialized
 * @param circleCenterX
 * @param circleCenterY
 * @param circleRadius
 */
void circleMask( double circleCenterX, double circleCenterY, double circleRadius )
{
    // Remember to call glEnable(GL_STENCIL_TEST) before calling this function
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_FALSE);
    glStencilFunc(GL_NEVER, 1, 0xFF);
    glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);  // draw 1s on test fail (always)
    // draw stencil pattern
    glStencilMask(0xFF);
    glClear(GL_STENCIL_BUFFER_BIT);  // needs mask=0xFF
    beginOrtho();
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glColor3f(0,0,0);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(circleCenterX, circleCenterY);
    for( int n = 0; n <= 100; ++n )
    {  float t = 2*3.141592654*(float)n/(float)100;
        glVertex2f(circleCenterX + sin(t)*circleRadius, circleCenterY + cos(t)*circleRadius);
    }
    glEnd();
    glPopAttrib();
    endOrtho();
    // END DRAW CIRCLE
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_TRUE);
    glStencilMask(0x00);
    // draw where stencil's value is 0
    glStencilFunc(GL_EQUAL, 0, 0xFF);
    /* (nothing to draw) */
    // draw only where stencil's value is 1
    glStencilFunc(GL_EQUAL, 1, 0xFF);
    // Remeber calling glDisable(GL_STENCIL_TEST) after this call
}

/**
 * @brief circleMask2  Fill with black the outside area of a circle centered in circleCenterx,circleCenterY with radius circleRadius.
 * @param circleCenterX
 * @param circleCenterY
 * @param circleRadius
 */
void circleMask2( double circleCenterX, double circleCenterY, double circleRadius )
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ZERO , GL_SRC_COLOR);
    beginOrtho();
    glBegin(GL_TRIANGLE_FAN);
    glColor3i(255,255,255);
    glVertex2f(circleCenterX, circleCenterY);
    for( int n = 0; n <= 100; ++n )
    {  float t = 2*3.141592654*(float)n/(float)100;
        glVertex2f(circleCenterX + sin(t)*circleRadius, circleCenterY + cos(t)*circleRadius);
    }
    glEnd();
    endOrtho();
    glPopAttrib();
}

/**
 * @brief project Project a 3D vector and get its (x,y) coordinates in viewport coordinates. The z coordinate is needed for correct depth-relations with other pixels
 * @param P
 * @return The projected (x,y,z) coordinates
 */
Eigen::Vector3d project(const Eigen::Vector3d &P)
{
    double x,y,z;
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    glGetIntegerv(GL_VIEWPORT,viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX,modelview);
    glGetDoublev(GL_PROJECTION_MATRIX,projection);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluProject(P.x(),P.y(),P.z(),modelview,projection,viewport,&x,&y,&z);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    y = viewport[3]-y;
    return Eigen::Vector3d(x,y,z);
}

/**
 * @brief drawCylinder
 * @param radius radius of cylinder
 * @param x0 start x coordinate of base
 * @param y0 start y coordinate of base
 * @param z0 start z coordinate of base
 * @param x1 end x coordinate
 * @param y1 end y coordinate
 * @param z1 end z coordinate
 * @param quality Quality (number of quads)
 * @param type (if type ==2 can be, cylinders are capped)
 */
void drawCylinder(double radius, double x0, double y0, double z0, double x1, double y1, double z1, int quality, int type)
{
    if(quality == 0) {
        glLineWidth(radius * 400);
        glBegin(GL_LINES);
        glVertex3f(x0,y0,z0);
        glVertex3f(x1,y1,z1);
        glEnd();
    } else {
        glPushMatrix();
        GLUquadricObj* quad = gluNewQuadric();
        glTranslatef(x0, y0, z0);
        double d[3];
        d[0] = x1 - x0;
        d[1] = y1 - y0;
        d[2] = z1 - z0;
        double d_norm = sqrt(d[0]*d[0]+d[1]*d[1]+d[2]*d[2]);
        double angle = 180.0 / M_PI * acosf(d[2]/d_norm);
        double r[3];
        r[0] = -d[1];
        r[1] = d[0];
        r[2] = 0;
        double r_norm = sqrt(r[0]*r[0]+r[1]*r[1]+r[2]*r[2]);
        r[0] /= r_norm;
        r[1] /= r_norm;
        r[2] /= r_norm;
        if(fabs(angle) < 0.001)
        {
        }
        else if(fabs(angle-180.0) < 0.001) {
        }
        else
            glRotatef(angle, r[0], r[1], r[2]);

        // parameters for gluCylinder: (quad, baseRadius, topRadius, height, slices, stacks);
        gluQuadricTexture(quad,true);
        //gluQuadricDrawStyle(quad,GLU_LINE);
        gluCylinder(quad, radius, type == 1 ? 0 : radius, d_norm, quality, 32);
        if(type == 2)
        {
            glTranslated(0.0, 0.0, d_norm/2);
            glutSolidCone(radius*2,radius*4, 20, 20);
        }
        gluDeleteQuadric(quad);
        glPopMatrix();
    }
}

/**
 * @brief drawCylinderElliptic
 * @param radiusX
 * @param heightY
 * @param radiusZ
 * @param ds
 */
void drawCylinderElliptic(double radiusX, double heightY, double radiusZ, double ds, bool fullCylinder)
{
    std::vector< std::vector<double> > vertices;
    double x=-radiusX+1E-8;
    double s=0.0;
    while ( x<= (radiusX-ds) )
    {
        double z = radiusZ*sqrt(1.0-pow(x/radiusX,2));
        double zDerivative = -radiusZ*x/(radiusX*radiusX)*(1.0/sqrt(1-(x/radiusX)*(x/radiusX)));
        double dx = ds/sqrt(1.0+zDerivative*zDerivative);
        x+=dx;
        s+=ds;
        vector<double> vals(4);
        vals[0] = x;
        vals[1]= heightY;
        vals[2]= z;
        vals[3]= s;
        vertices.push_back(vals);
    }

    glBegin(GL_QUADS);
    x=-radiusX+ds;
    double circ = mathcommon::ellipseCircumferenceBetter(radiusX,radiusZ);
    for (unsigned int i=0; i<vertices.size()-1; i++)
    {
        // Texture coordinates are generated along the circumference of the ellipse being calculated
        double vertexArcLength =vertices[i][3]/(0.5*circ);
        glTexCoord2d( vertexArcLength , 1 );
        glVertex3dv(&vertices[i][0]);

        glTexCoord2d( vertexArcLength, 0);
        glVertex3d(vertices[i][0],-heightY,vertices[i][2]);

        glTexCoord2d( vertexArcLength,0);
        glVertex3d(vertices.at(i+1)[0],-heightY,vertices.at(i+1)[2]);

        glTexCoord2d( vertexArcLength ,1);
        glVertex3dv(&vertices.at(i+1)[0]);
    }
    glEnd();
}

/**
 * @brief drawParaboloidConstantSurfaceElement
 * @param xMin
 * @param xMax
 * @param zMin
 * @param zMax
 * @param nStepsX
 * @param nStepsZ
 */
void drawParaboloidConstantSurfaceElement(double xMin, double xMax, double zMin, double zMax, int nStepsX, int nStepsZ)
{
    Grid grid;
    grid.init(Eigen::Hyperplane<double,3>(Vector3d(0,1,0),0.0),xMax-xMin);
    grid.setRowsAndCols(nStepsX,nStepsZ);
    grid.draw(true);
}

/**
 * @brief glError Query GL status and get an error, the error with name a filename+line is printed on standard error
 * @param file
 * @param line
 */
void glError(const char *file, int line)
{
    GLenum glErr;
    int retCode = 0;

    glErr = glGetError();
    while (glErr != GL_NO_ERROR)
    {  const GLubyte* sError = gluErrorString(glErr);

        if (sError)
            cerr << "GL Error #" << glErr << "(" << gluErrorString(glErr) << ") " << " in File " << file << " at line: " << line << endl;
        else
            cerr << "GL Error #" << glErr << " (no message available)" << " in File " << file << " at line: " << line << endl;

        retCode = 1;
        glErr = glGetError();
    }
}

/**
 * @brief drawEllipse
 * @param radiusX
 * @param radiusY
 * @param focalDistance
 * @param minAngle
 * @param maxAngle
 */
void drawEllipse(double radiusX, double radiusY,double focalDistance, double minAngle, double maxAngle)
{
    double stepsize=(maxAngle-minAngle)/100;
    glBegin(GL_LINE_STRIP);
    for (double t=minAngle; t<=maxAngle;  t+=stepsize )
    {
        glVertex3d(radiusX*cos(t),radiusY*sin(t),focalDistance);
    }
    glEnd();
}
/**
 * @brief loadTexture
 * @param imageNameFile
 * @param internalFormat
 * @return
 */
GLuint loadTexture(std::string &imageNameFile, GLint internalFormat)
{
    Image *image = loadBMP(imageNameFile.c_str());
    GLuint textureId=-1;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 internalFormat,    //switch to GL_LUMINANCE for grayscale images
                 image->width, image->height,
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 image->pixels);
    gluBuild2DMipmaps(textureId, GL_LUMINANCE,  image->width, image->height, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glGetError();
    delete image;
    return textureId;
}

/**
 * @brief generateCircles
 * @param width
 * @param height
 * @param ncircles
 * @param minRadius
 * @param maxRadius
 * @return
 */
GLubyte* generateCircles(int width, int height, int ncircles, int minRadius, int maxRadius, bool addNoise, double noiseDensity, bool usePolkaDots)
{
    int depth=4;
    // VERY IMPORTANT SETTING PIXEL PACK ALIGNMENT!!!
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    GLubyte * M = new GLubyte[depth*width*height];
    for (int i=0; i<depth*width*height; i++)
        M[i]=0;
    if ( usePolkaDots )
    {
        int totalArea = width*height;
        double occupiedArea=0.0;
        // This is a bogus algorithm to avoid intersecting circles, it can be done better by means
        // of intersection graph and maximal independent set but it need a lot of more code!!
        std::map<int,Circle<int> > circles;
        int radius = mathcommon::unifRand(minRadius,maxRadius);
        circles[0]=Circle<int>( mathcommon::unifRand(radius,width-radius),mathcommon::unifRand(radius,height-radius),radius);
        int noncoll=0;
        while (noncoll < ncircles && occupiedArea<totalArea )
        {
            int radius = mathcommon::unifRand(minRadius,maxRadius);
            int centerx = mathcommon::unifRand(radius,width-radius);
            int centery = mathcommon::unifRand(radius,height-radius);
            Circle<int> tmp(centerx,centery,radius);
            bool tmpIsCollidingWithSomeOtherElement=false;
            for (map<int, Circle<int> >::iterator iter = circles.begin(); iter!=circles.end(); ++iter)
            {
                if  ( iter->second.checkCollision(tmp,5) )
                {
                    tmpIsCollidingWithSomeOtherElement=true;
                    break;
                }
            }
            if ( !tmpIsCollidingWithSomeOtherElement )
            {
                circles[noncoll++]=tmp;
                occupiedArea+=3.141592654*radius*radius;
            }
        }

        for (unsigned int i=0; i<circles.size(); i++)
        {
            int radius= circles[i].radius;
            int centerx = circles[i].centerx;
            int centery = circles[i].centery;

            for (int x=- radius+ centerx; x<=radius+centerx; x++)
            {
                for (int y=-radius+centery; y<=radius+centery; y++)
                {
                    if ( SQR((x-centerx)) + SQR((y-centery)) < SQR(radius) )
                        M[x+y*width]=255;
                }
            }
        }
    }
    // add some random noise
    if ( addNoise )
    {
        int radius=10;
        int nDots = (width*height)*noiseDensity/(double)(radius*radius*M_PI);
        /*
        // Pregenerate singleton of points so that no repetitions are possible
        vector<int> centersX(nDots),centersY(nDots);
        for (int i=radius+1; i<height-radius-1; i++)
            centersX[i]=i;
        for (int i=radius+1; i<width-radius-1; i++)
            centersY[i]=i;

        std::random_shuffle(centersX.begin(),centersX.end());
        std::random_shuffle(centersY.begin(),centersY.end());
        */
        for (int i=0; i<nDots; i++)
        {
            int centerx = mathcommon::unifRand(radius,width-radius);
            int centery = mathcommon::unifRand(radius,height-radius);
            for (int x=- radius+ centerx; x<=radius+centerx; x++)
            {
                for (int y=-radius+centery; y<=radius+centery; y++)
                {
                    if ( SQR((x-centerx)) + SQR((y-centery)) < SQR(radius) )
                        M[x+y*width]=255;
                }
            }
        }
    }
    return M;
}


/**
 * @brief generateCheckerboard
 * @param width
 * @param height
 * @param tileSquareSize
 * @return
 */
GLubyte* generateCheckerboard(int width, int height, int tileSquareSize)
{
    int depth=4;
    // VERY IMPORTANT SETTING PIXEL PACK ALIGNMENT!!!
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    GLubyte * M = new GLubyte[depth*width*height];
    for (int i=0; i<depth*width*height; i++)
        M[i]=0;

    for (int i=0; i<width; i++)
    {
        for (int j=0; j<height; j++)
        {
            M[i+j*width] = (i+j)%tileSquareSize < tileSquareSize/2 ? 0: 255;
        }
    }
    return M;
}
/**
 * @brief generateCheckerboardTexture
 * @param width
 * @param height
 * @param tileWidth
 * @param tileHeight
 * @return
 */
GLuint generateCheckerboardTexture(int width, int height, int tileWidth, int tileHeight)
{
    GLubyte  *image = generateCheckerboard(width,height,tileWidth);

    GLuint textureId=0;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_LUMINANCE,
                 width,
                 height,
                 0,
                 GL_LUMINANCE,
                 GL_UNSIGNED_BYTE,
                 (GLvoid*)image);

    gluBuild2DMipmaps(textureId, GL_LUMINANCE,  width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, image);
    glGetError();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    delete[] image;
    return textureId;
}
/**
 * @brief generateCirclesTexture
 * @param width
 * @param height
 * @param ncircles
 * @param minRadius
 * @param maxRadius
 * @param isSpaceFilling
 * @return
 */
GLuint generateCirclesTexture(int width, int height, int ncircles, int minRadius, int maxRadius, bool isSpaceFilling, bool addRandomNoise, double noiseDensity, bool usePolkaDots)
{
    GLubyte  *image;
    if (isSpaceFilling)
        image = generateCirclesSpaceFilling(width,height);
    else
        image = generateCircles(width,height,ncircles,minRadius,maxRadius,addRandomNoise,noiseDensity,usePolkaDots);

    GLuint textureId=0;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_LUMINANCE,
                 width,
                 height,
                 0,
                 GL_LUMINANCE,
                 GL_UNSIGNED_BYTE,
                 (GLvoid*)image);

    gluBuild2DMipmaps(textureId, GL_LUMINANCE,  width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, image);
    glGetError();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    delete[] image;
    return textureId;
}

/*
http://paulbourke.net/texture_colour/randomtile/

choose value of c
calculate initial area from Riemann zeta relationship
initiate random number generator
repeat for i=0 to some chosen number of iterations n
   area of new object = initial area multiplied by pow(i,-c)
   calculate the dimensions of the new object given the area of the new object
   repeat
      choose a random position in the region of the plane being filled
      check for intersection of the new object at this position with all other objects
      if the new object does not intersect exit the repeat loop
   end repeat
   add the new new object to the plane
end repeat
 */
/**
 * @brief generateCirclesSpaceFilling
 * @param width
 * @param height
 * @return
 */
GLubyte* generateCirclesSpaceFilling(int width, int height)
{
    int depth=4;
    // VERY IMPORTANT SETTING PIXEL PACK ALIGNMENT!!!
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    GLubyte * M = new GLubyte[depth*width*height];
    for (int i=0; i<depth*width*height; i++)
        M[i]=0;

    int totalArea = width*height;
    //std::map<int,Circle<int> > circles;
    std::vector< Circle<int> > circles(1);
    double c=1.2;
    double initialArea = (double)(totalArea)/boost::math::zeta<double>(c);
    int radius = sqrt(initialArea/M_PI);
    circles[0]=Circle<int>( mathcommon::unifRand(radius,width-radius),mathcommon::unifRand(radius,height-radius),radius);

    //while (noncoll < ncircles && occupiedArea<totalArea )
    double totalAreaCovered = 0.0;
    for (int iteration=0; iteration<2000; iteration++)
        //int iteration=0;
        //while ( (double)totalAreaCovered/(double)totalArea < 0.6 )
    {
        double newArea = initialArea*pow((double)iteration,-c);
        int radius = (int)floor(sqrt(newArea/M_PI));
        for (int k=0; k<200; k++)
            //while (true)
        {
            int centerx = mathcommon::unifRand(radius+1,width-radius-1);
            int centery = mathcommon::unifRand(radius+1,height-radius-1);
            Circle<int> tmp(centerx,centery,radius);
            bool tmpIsCollidingWithSomeOtherElement=false;
            for (unsigned int n=0; n<circles.size(); n++)
            {
                tmpIsCollidingWithSomeOtherElement = tmpIsCollidingWithSomeOtherElement || circles[n].checkCollision(tmp,5);
            }
            if (!tmpIsCollidingWithSomeOtherElement)
            {
                circles.push_back(tmp);
                totalAreaCovered+=tmp.radius*tmp.radius*M_PI;
                break;
            }
        }
        //iteration++;
    }

    //cerr << (double)totalAreaCovered/(double)totalArea*100 << " % covered" << endl;

    // Fill the texture with the circles
    for (unsigned int i=0; i<circles.size(); i++)
    {
        int radius= circles.at(i).radius;
        int centerx = circles.at(i).centerx;
        int centery = circles.at(i).centery;

        for (int x=- radius+ centerx; x<=radius+centerx; x++)
        {
            for (int y=-radius+centery; y<=radius+centery; y++)
            {
                if ( SQR((x-centerx)) + SQR((y-centery)) < SQR(radius) )
                    M[x+y*width]=255;
                if ( SQR((x-centerx)) + SQR((y-centery)) == SQR(radius) )
                    M[x+y*width]=128;
            }
        }
    }
    return M;
}

void drawEllipsoid(double x0, double y0, double z0, double x1, double y1, double z1)
{
    double d[3];
    d[0] = x1 - x0;
    d[1] = y1 - y0;
    d[2] = z1 - z0;

    double d_norm = sqrt(d[0]*d[0]+d[1]*d[1]+d[2]*d[2]);
    double angle = 180.0 / M_PI * acosf(d[0]/d_norm);
    double r[3];
    r[0] = d[0];
    r[1] = d[1];
    r[2] = 0;
    double r_norm = sqrt(r[0]*r[0]+r[1]*r[1]+r[2]*r[2]);
    r[0] /= r_norm;
    r[1] /= r_norm;
    r[2] /= r_norm;

    Eigen::Vector3d dd(x1-x0,y1-y0,z1-z0);
    //dd.normalize();

    glPushMatrix();
    glTranslatef(d[0]/2,d[1]/2,d[2]/2);
    glRotatef(angle, r[0], r[1], r[2]);
    glScalef( dd[0], dd[1], dd[2]);
    glutSolidSphere( dd.norm()/2-0.5, 40, 40 );
    glPopMatrix();
}

void drawEllipsoid(const Vector3d &p0, const Vector3d &p1)
{
    drawEllipsoid(p0.x(),p0.y(),p0.z(),p1.x(),p1.y(),p1.z());
}

void drawFrustum(const double * projectionMatrix, const  double *inverseModelViewMatrix)
{
    // Get near and far from the Projection matrix.
    const double near = projectionMatrix[11] / (projectionMatrix[10] - 1.0);
    const double far = projectionMatrix[11] / (1.0 + projectionMatrix[10]);
    // Get the sides of the near plane.
    const double nLeft = near * (projectionMatrix[2] - 1.0) / projectionMatrix[0];
    const double nRight = near * (1.0 + projectionMatrix[2]) / projectionMatrix[0];
    const double nTop = near * (1.0 + projectionMatrix[6]) / projectionMatrix[5];
    const double nBottom = near * (projectionMatrix[6] - 1.0) / projectionMatrix[5];
    // Get the sides of the far plane.
    const double fLeft = far * (projectionMatrix[2] - 1.0) / projectionMatrix[0];
    const double fRight = far * (1.0 + projectionMatrix[2]) / projectionMatrix[0];
    const double fTop = far * (1.0 + projectionMatrix[6]) / projectionMatrix[5];
    const double fBottom = far * (projectionMatrix[6] - 1.0) / projectionMatrix[5];

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMultMatrixd(inverseModelViewMatrix);
    glPushMatrix();
    glScaled(20,20,20);
    drawCamera();
    glPopMatrix();
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex3d(0.0f, 0.0f, 0.0f);
    glVertex3d(fLeft, fBottom, -far);
    glVertex3d(0.0f, 0.0f, 0.0f);
    glVertex3d(fRight, fBottom, -far);
    glVertex3d(0.0f, 0.0f, 0.0f);
    glVertex3d(fRight, fTop, -far);
    glVertex3d(0.0f, 0.0f, 0.0f);
    glVertex3d(fLeft, fTop, -far);
    //far
    glVertex3d(fLeft, fBottom, -far);
    glVertex3d(fRight, fBottom, -far);
    glVertex3d(fRight, fTop, -far);
    glVertex3d(fLeft, fTop, -far);
    glVertex3d(fRight, fTop, -far);
    glVertex3d(fRight, fBottom, -far);
    glVertex3d(fLeft, fTop, -far);
    glVertex3d(fLeft, fBottom, -far);
    //near
    glVertex3d(nLeft, nBottom, -near);
    glVertex3d(nRight, nBottom, -near);
    glVertex3d(nRight, nTop, -near);
    glVertex3d(nLeft, nTop, -near);
    glVertex3d(nLeft, nTop, -near);
    glVertex3d(nLeft, nBottom, -near);
    glVertex3d(nRight, nTop, -near);
    glVertex3d(nRight, nBottom, -near);

    // Central ray
    glVertex3d(0,0,0);
    glVertex3d(0,0,-far);
    glEnd();
    glLineWidth(1);
    glPopMatrix();
}

/**
 * @brief drawFrustum
 * @param ProjectionMatrix
 * @param InverseModelViewMatrix
 */
void drawFrustum(const Eigen::Projective3d &ProjectionMatrix, const Affine3d &InverseModelViewMatrix)
{
    drawFrustum(ProjectionMatrix.matrix().data(),InverseModelViewMatrix.matrix().data());
}

/**
 * @brief drawCamera
 */
void drawCamera()
{
    // start to render polygons
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);

    glNormalPointer(GL_FLOAT, 0, cameraNormals);
    glVertexPointer(3, GL_FLOAT, 0, cameraVertices);

    glDrawElements(GL_TRIANGLE_STRIP, 5, GL_UNSIGNED_INT, &cameraIndices[0]);
    glDrawElements(GL_TRIANGLE_STRIP, 5, GL_UNSIGNED_INT, &cameraIndices[5]);
    glDrawElements(GL_TRIANGLE_STRIP, 5, GL_UNSIGNED_INT, &cameraIndices[10]);
    glDrawElements(GL_TRIANGLE_STRIP, 5, GL_UNSIGNED_INT, &cameraIndices[15]);
    glDrawElements(GL_TRIANGLE_STRIP, 5, GL_UNSIGNED_INT, &cameraIndices[20]);
    glDrawElements(GL_TRIANGLE_STRIP, 5, GL_UNSIGNED_INT, &cameraIndices[25]);
    glDrawElements(GL_TRIANGLE_STRIP, 39, GL_UNSIGNED_INT, &cameraIndices[30]);
    glDrawElements(GL_TRIANGLE_STRIP, 44, GL_UNSIGNED_INT, &cameraIndices[69]);
    glDrawElements(GL_TRIANGLE_STRIP, 44, GL_UNSIGNED_INT, &cameraIndices[113]);
    glDrawElements(GL_TRIANGLE_STRIP, 44, GL_UNSIGNED_INT, &cameraIndices[157]);
    glDrawElements(GL_TRIANGLE_STRIP, 44, GL_UNSIGNED_INT, &cameraIndices[201]);

    glDisableClientState(GL_VERTEX_ARRAY);	// disable vertex arrays
    glDisableClientState(GL_NORMAL_ARRAY);	// disable normal arrays
}
