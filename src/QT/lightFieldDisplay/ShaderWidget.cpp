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
#include <QtOpenGL/QGLShaderProgram>
#include "ShaderWidget.h"
#include "Homography.h"
#include "ObjLoader2.h"
#include "Util.h"
#define GL_FRAGMENT_PRECISION_HIGH 1

#ifdef __APPLE__
QString CNCSVISION_BASE_DIRECTORY("/Users/rs/workspace/cncsvisioncmake/");
#endif

#ifdef __linux__
QString CNCSVISION_BASE_DIRECTORY("/home/carlo/workspace/cncsvisioncmake/");
#endif

#ifdef WIN32
QString CNCSVISION_BASE_DIRECTORY("C:/workspace/cncsvisioncmake/");
#endif


#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

#define SQRT2OVER2 0.70710678118654752440084436210485

// This is helpful to understand how to draw off-screen with OpenGL
// http://stackoverflow.com/questions/14785007/can-i-use-opengl-for-off-screen-rendering?rq=1

// For issues about OpenGL inclusion when glew conflicts with OpenGL I suggest to look:
// http://stackoverflow.com/questions/15048729/where-is-glgenbuffers-in-qt5
/**
 * @brief ShaderWidget::ShaderWidget
 * @param parent
 * @param shareWidget
 */
ShaderWidget::ShaderWidget(QWidget *parent) :
    QGLWidget(parent)
{
    obj = new ObjLoader2();
    shaderProgram=NULL;
    glDisable(GL_MULTISAMPLE);
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glDisable(GL_SMOOTH);
    glDisable(GL_LINE_SMOOTH);

    // First load the 2D and 3D points
    //QMessageBox::information(0, "2D points selection", "Please select the 2D points file for the calibration. The points file must contain just two columns of pixel coordinates.");

    QString points2Dfilename = "I:/Images/23July2013/2D_points.txt";
    //QString points2Dfilename = "/home/carlo/Desktop/JORDANKA/Calibrations/2D_points.txt";
    //QFileDialog::getOpenFileName(this,"Select 2D projected points correspondence",QDir::currentPath(),"*.txt");

    if (points2Dfilename.isEmpty())
    {
        QMessageBox::critical(this,"Error","Select a valid (non-empty) file for 2D points. Now exiting...");
        QCoreApplication::exit(-1);
    }
    QFileInfo fileInfo2dpoints(points2Dfilename);
    //QMessageBox::information(this, "3D points selection", "Please select the corresponding 3D points file for the calibration. The points file must contain just three columns of world coordinates.\nThe number of 3D points must be the same of the 2D points.");

    QString points3Dfilename = "I:/Images/23July2013/3D_points.txt";
    // QFileDialog::getOpenFileName(this,"Select 3D projected points correspondence", fileInfo2dpoints.absolutePath(),"*.txt");

    if ( points3Dfilename.isEmpty() )
    {
        QMessageBox::critical(this,"Error","Select a valid (non-empty)  file for 3D points. Now exiting...");
        QCoreApplication::exit(-1);
    }

    //QMessageBox::information(this, "Output images", "Please select the output path for the generated images");

    //QString outputFileName = "I:/Images/05July2013/images/tmp/img";//QFileDialog::getSaveFileName(NULL,"Select output images path",fileInfo2dpoints.absolutePath());
    //QString outputFileName = "I:/Images/05July2013/images/tmp/img";//QFileDialog::getSaveFileName(NULL,"Select output images path",fileInfo2dpoints.absolutePath());
    QString outputFileName = "I:/Images/23July2013/images/img";
    if ( outputFileName.isEmpty() )
    {
        QMessageBox::critical(this,"Error","Select a valid (non-empty) file name for output points. Now exiting...");
        QCoreApplication::quit();
    }

    QFileInfo outputFileInfo(outputFileName);
    this->folderName = outputFileInfo.absolutePath();
    this->imagesFilesFolderName = outputFileInfo.absolutePath();
    this->imagesFileBaseName = outputFileInfo.baseName();

    double zNear = 0.1;
    double zFar = 100;
    int width = 604;
    int height = 608;
    bool decomposePMatrix = true;
    bool computeOpenGLMatrices = true;
    CameraDirectLinearTransformation cam(points2Dfilename.toStdString(),points3Dfilename.toStdString(),true,true,0,0,width,height,zNear,zFar);

    this->Projection.resize(4,4);
    this->ModelView.resize(4,4);
    this->Projection << cam.getOpenGLProjectionMatrix().matrix();
    this->ModelView << cam.getOpenGLModelViewMatrix().matrix();

    cerr << this->Projection.matrix() << endl;
    cerr << this->ModelView.matrix() << endl;
    //QMessageBox::information(this, "Wavefront Object loading", "Please select the 3D mesh in Wavefront OBJ format to be loaded.");
    //QString objFileName = "C:/workspace/cncsvisioncmake/data/objmodels/face.obj";
    //QString objFileName = "/home/carlo/workspace/cncsvisioncmake/data/objmodels/head_decimated.obj";
	QString objFileName = "C:/workspace/cncsvisioncmake/data/objmodels/face.obj";
    //QFileDialog::getOpenFileName(this,"Select Wavefront OBJ mesh file",QDir::currentPath(),"*.obj");
    qDebug() << objFileName ;
    if (objFileName.isEmpty())
    {
        QMessageBox::critical(this,"Error","Select a valid (non-empty) file name for OBJ model to load.");
        QCoreApplication::exit(-1);
    }
    this->loadObjFile(objFileName.toStdString());
    //QMessageBox::information(this, "Wavefront Object loading", "Please select the projector parameters file.");
    //QString projectorParametersFileName = "I:/Images/05July2013/ProjectoParameters.txt";
    QString projectorParametersFileName = "I:/Images/23July2013/ProjectorParameters.txt";
    //QFileDialog::getOpenFileName(this,"Select projector parameters file name",fileInfo2dpoints.absolutePath(),"*.txt");
    if (projectorParametersFileName.isEmpty())
    {
        QCoreApplication::exit(-1);
    }
    this->projectorParameters.loadParameterFile(projectorParametersFileName.toStdString());

    // Typical parameters are
    // ProjectorAreaWidth 604
    // ProjectorAreaHeight 684
    // ProjectionHeightMM 250
    // ViewerDistanceMM : 500
    // ViewerHeightMM: 0.0
    // SlicesNumber: 96
    this->projectorPixelWidth =604;
    this->projectorPixelHeight= 608;
    this->projectorHeight= 250.0;
    this->viewerDistance= 500.0;
    this->viewerHeight= 0.0;
    this->viewsNumber=96;
	this->objectScale=10.0;
	
    try
    {
        this->projectorHeight= util::str2num<double>(this->projectorParameters.find("ProjectorHeightMM") );
        this->viewerDistance= util::str2num<double>(this->projectorParameters.find("ViewerDistanceMM") );
        this->viewerHeight= util::str2num<double>(this->projectorParameters.find("ViewerHeightMM") );
        this->viewsNumber= util::str2num<int>(this->projectorParameters.find("ViewsNumber") );
        this->objectScale= util::str2num<double>(this->projectorParameters.find("ObjectScale") );
    }
    catch ( std::invalid_argument &e )
    {
        cerr << e.what() << endl;
        QMessageBox::critical(this,"Parameters files error ",e.what());
        QCoreApplication::exit(-1);
    }

	mirrorAngleRad=M_PI;
	
    this->resize(projectorPixelWidth,projectorPixelHeight);
    glPointSize(0.1f);
    glLineWidth(0.1f);
    QTimer *timer = new QTimer(this);
    timer->start(15);
    QObject::connect(timer,SIGNAL(timeout()),this,SLOT(repaint()));
    this->updateView();
    this->setFocus();
    frameNumber=0; //very important to count the number of frames
}

/**
 * @brief ShaderWidget::~ShaderWidget
 */
ShaderWidget::~ShaderWidget()
{
    //delete this->obj;
}

/**
 * @brief ShaderWidget::minimumSizeHint
 * @return
 */
QSize ShaderWidget::minimumSizeHint() const
{
    return QSize(PROJECTOR_WIDTH,PROJECTOR_HEIGHT);
}

/**
 * @brief ShaderWidget::sizeHint
 * @return
 */
QSize ShaderWidget::sizeHint() const
{
    return QSize(PROJECTOR_WIDTH,PROJECTOR_HEIGHT);
}

/**
 * @brief ShaderWidget::loadObjFile
 * @param objFileName
 */
void ShaderWidget::loadObjFile(const std::string &objFileName)
{
    QFile file(QString::fromStdString(objFileName));
    if (file.exists())
    {
        char *filename =( char*)objFileName.c_str();
        try
        {
            obj->load(objFileName);
            obj->normalizeToUnitBoundingBox();
            obj->setFillColor(0.0,0.0,0.0,1.0);
            obj->setLineColor(1.0,1.0,1.0,1.0);
        }
        catch (std::exception &e)
        {
            cerr << e.what() << endl;
            QMessageBox::critical(this,"File not found", e.what() );
        }
    }
    else
    {
        qDebug() << "File not found" ;
        QMessageBox::critical(this,"File not found", "Error "+ QString::fromStdString(objFileName) + " not found" );
        QCoreApplication::exit(-1);
    }
    if (obj->getObjModel().empty())
    {
        qDebug() << "File " << QString(objFileName.c_str()) << " empty!";
        QMessageBox::critical(this,"Error", "Empty model or input file error" );
        QCoreApplication::exit(-1);
    }
}

/**
 * @brief ShaderWidget::loadShader
 * @param _vertexShaderFile
 * @param _fragmentShaderFile
 * @param textureFile
 */
void ShaderWidget::initializeShader()
{
    static char const vertexShader[] = STATIC_STRINGIFY
            (
                uniform highp vec3 reflectedProjector;
            uniform highp vec3 mirrorNormal;
    uniform highp vec3 mirrorCenter;
    uniform highp float viewerDistance;
    uniform highp float viewerHeight;
    uniform highp float scale;
    uniform highp float meshColor;
    vec3 getRayCylinderIntersection(  vec3 rayDirection,  vec3 rayOrigin, float cylinderRadius )
    {
        float a = rayDirection.x*rayDirection.x+rayDirection.z*rayDirection.z;
        float b = 2.0*(rayDirection.x*rayOrigin.x+rayOrigin.z*rayDirection.z);
        float c = rayOrigin.x*rayOrigin.x+rayOrigin.z*rayOrigin.z-cylinderRadius*cylinderRadius;
        float t = (-b+sqrt(b*b-4.0*a*c))/(2.0*a);
        return rayDirection*t+rayOrigin;
    }
    vec3 getRayPlaneIntersection( vec3 D,  vec3 E,  vec3 N,  vec3 Q)
    {
        return (dot(N,Q-E)/dot(N,D))*D+E;
    }
    void main(void)
    {
        vec3 Q = gl_Vertex.xyz*scale;
        // define ray from reflected projector position P’ to vertex Q
        vec3 PQ = normalize(Q - reflectedProjector);
        // compute intersection of ray PQ with vertical cylinder with radius d to find view position V’
        vec3 V = getRayCylinderIntersection(PQ,Q,viewerDistance);
        // set correct viewer height
        V.y = viewerHeight;
        // define ray from ideal viewing position V’ to vertex Q
        vec3 VQ = normalize(Q - V);
        //n.xyz=vec3(1,0,0);
        // compute intersection ray VQ with mirror plane to find point M
        vec3 M = getRayPlaneIntersection(VQ,V,mirrorNormal, mirrorCenter);
        vec4 outputQ = ( gl_ModelViewProjectionMatrix*vec4(M.xyz,1.0) ); // project M into projector
        //vec4 outputQ = ( MV*Proj*vec4(M.xyz,1.0) ); // project M into projector
        // keep the existing vertex color, recompute depth in based on distance from V’
        outputQ.z = length(V -Q) / (2.0*length(V - M));
        gl_Position = outputQ;
    }
    );

    static char const fragmentShader[] = STATIC_STRINGIFY(
                uniform highp float meshColor;
                void main(void)
    {
                    gl_FragColor = vec4(meshColor);
                }
                );

    if ( shaderProgram!=NULL )
        delete shaderProgram;

    // Create the new shader programs
    this->makeCurrent();
    this->shaderProgram = new QGLShaderProgram(this);

    this->shaderProgram->addShaderFromSourceCode(QGLShader::Vertex,vertexShader);
    this->shaderProgram->addShaderFromSourceCode(QGLShader::Fragment,fragmentShader);

    if ( ! shaderProgram -> link() )
        qWarning () << shaderProgram -> log () << endl;
}

/**
 * @brief ShaderWidget::initializeGL
 */
void ShaderWidget::initializeGL()
{
    int argc = 1;
    char *argv[] = {""};

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH ); // Set up a basic display buffer (only single buffered for now)
    
    qglClearColor(Qt::black);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set depth buffer clear value
    glClearDepth(1.0);
    // Enable depth test */
    glEnable(GL_DEPTH_TEST);
    // Set depth function
    glDepthFunc(GL_LEQUAL);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    this->initializeShader();
    this->makeCurrent();
    glewInit();
    qDebug() << "Initing buffers..." ;
    obj->initializeBuffers();
    qDebug()<< "OBJ initialized " ;
}

/**
 * @brief ShaderWidget::updateUniforms
 */
void ShaderWidget::updateView()
{
    this->mirrorNormal << SQRT2OVER2*sin(mirrorAngleRad) , SQRT2OVER2, SQRT2OVER2*cos(mirrorAngleRad);
    this->projector << 0,projectorHeight,0;
    this->mirrorCenter<< 0,0,0;
    this->reflectedProjector << getReflectedRay(this->projector,this->mirrorNormal);
    mirrorAngleRad+=M_PI*2/viewsNumber;
}

/**
 * @brief ShaderWidget::drawCylinder
 * @param mode
 * @param radius
 * @param nSlices
 * @param nStacks
 */
void ShaderWidget::drawCylinder(int mode, double radiusX, double radiusZ, double height, int nSlices, int nStacks)
{
    /*
    glPushMatrix();
    // Generate the points on the cylinder
    for ( double theta = -M_PI; theta<=0; theta+=0.2 )
    {
        glBegin(GL_LINE_STRIP);
        for (double y = -height; y<=height;y+=0.1 )
        {
            glVertex3d( radiusX*cos(theta), y, radiusZ*sin(theta) );
        }
        glEnd();
    }
    glPopMatrix();
    */
    glBegin(GL_POINTS);
    glVertex3d(0,0,0);
    glEnd();
}

/**
 * @brief ShaderWidget::drawSphere
 * @param mode
 * @param radius
 * @param nSlices
 * @param nStacks
 */
void ShaderWidget::drawSphere(int mode, int radius, int nSlices, int nStacks)
{
    glPushMatrix();
    GLUquadric *quad = gluNewQuadric();
    glPolygonMode(GL_FRONT_AND_BACK,mode);
    gluSphere(quad,radius,nSlices,nStacks);
    gluDeleteQuadric(quad);
    glPopMatrix();
}

/**
 * @brief ShaderWidget::drawGpuComputedM
 */
void ShaderWidget::drawGpuComputedM()
{
	int objectToDraw=DRAW_MODEL;

    ////// DRAW THE ACTUAL VERTICES //////
    glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(Projection.data());  // load the modelviewprojection matrix computed by pair correspondence
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixd(ModelView.data());
    // Now begins the shader with only the vertices

    this->shaderProgram->link();
    this->shaderProgram->bind();
    this->shaderProgram->setUniformValue("scale",(GLfloat)this->objectScale);
    this->shaderProgram->setUniformValue("reflectedProjector",reflectedProjector.x(),reflectedProjector.y(),reflectedProjector.z());
    this->shaderProgram->setUniformValue("mirrorCenter",mirrorCenter.x(),mirrorCenter.y(),mirrorCenter.z());
    this->shaderProgram->setUniformValue("viewerDistance",(GLfloat)viewerDistance);
    this->shaderProgram->setUniformValue("viewerHeight",(GLfloat)viewerHeight);
    this->shaderProgram->setUniformValue("mirrorNormal",mirrorNormal.x(),mirrorNormal.y(),mirrorNormal.z());
    this->shaderProgram->setUniformValue("meshColor",(GLfloat)1.0f);
    switch ( objectToDraw )
    {
    case DRAW_MODEL:
    {
        // Draw the vertices in front then the faces as black and opaque
        // so that they cover the points on the back
        glPushMatrix();
        obj->draw( GL_POINT ); //draw the object with full black faces
        glPolygonOffset(1.0f, 1.0f);
        glEnable(GL_POLYGON_OFFSET_FILL);
        this->shaderProgram->setUniformValue("meshColor",(GLfloat)0.0f);
        obj->draw( GL_FILL ); //draw the object with full black faces
        glDisable(GL_POLYGON_OFFSET_FILL);
        glPopMatrix();
       break;
	} 
	case DRAW_TEAPOT:
		{
			glPushMatrix();
			glutWireTeapot(1.0);
			glPolygonOffset(1.0f, 1.0f);
			glEnable(GL_POLYGON_OFFSET_FILL);
			this->shaderProgram->setUniformValue("meshColor",(GLfloat)0.0f);
			glutSolidTeapot(1.0);
			glDisable(GL_POLYGON_OFFSET_FILL);
			glPopMatrix();
		}
		break;
		case DRAW_BALL:
		{
			glPushMatrix();
			glBegin(GL_LINE_LOOP);
			glVertex3d(-1,0,1);
			glVertex3d(1,0,1);
			glVertex3d(1,0,-1);
			glVertex3d(-1,0,-1);
			glEnd();

			glBegin(GL_LINE_LOOP);
			glVertex3d(-1,0,1);
			glVertex3d(1,1,1);
			glVertex3d(1,1,-1);
			glVertex3d(-1,0,-1);
			glEnd();

			glBegin(GL_LINE_LOOP);
			glVertex3d(1,0,1);
			glVertex3d(1,0,-1);
			glVertex3d(1,1,-1);
			glVertex3d(1,1,1);
			glEnd();

			glPopMatrix();
		}
		break;
    }

    this->shaderProgram->release();
    glPopMatrix();
}

/**
* @brief ShaderWidget::drawFunction
*/
void ShaderWidget::drawFunction()
{
    glPushMatrix();
    glBegin(GL_POINTS);
    for (double x=-2; x<2; x+=0.1)
    {
        for (double  z=-2; z<2; z+=0.1)
        {
            glVertex3d(x,0.5*(x*x+z*z)-2.5,z);
        }
    }
    glEnd();
    glPopMatrix();
}

/**
 * @brief ShaderWidget::paintGL
 */
void ShaderWidget::paintGL()
{
    qglClearColor(Qt::black);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ////////////////////////////////////////
    drawGpuComputedM();
    saveFrame();
    updateView();
    ////////////////////////////////////////
}

/**
 * @brief ShaderWidget::saveFrame
 */
void ShaderWidget::saveFrame()
{
    QString frameString;
    frameString.sprintf("%03d", frameNumber);
    if ( frameNumber >= viewsNumber )
    {
        return;
    }
    frameNumber++;
    QString outputFrameFileName = imagesFilesFolderName + QString(QDir::separator())+ this->imagesFileBaseName + "_" + frameString +".bmp";
    //qDebug() <<outputFrameFileName  ;
    QImage frame = this->grabFrameBuffer();

    int dpm = ceil(72.0 / 0.0254); // ~300 DPI
    frame.setDotsPerMeterX(dpm);
    frame.setDotsPerMeterY(dpm);
    frame = frame.convertToFormat(QImage::Format_Mono);
    frame.save( outputFrameFileName, NULL, -1);
}

/**
 * @brief ShaderWidget::resizeGL
 * @param width
 * @param height
 */
void ShaderWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, (GLsizei) width, (GLsizei) height); // Set our viewport to the size of our window
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float FOV=90;
    gluPerspective(FOV, (float)width / (float)height, 0.001, 10000.0);
    glMatrixMode(GL_MODELVIEW);
}


Eigen::Vector3d ShaderWidget::getReflectedRay(const Eigen::Vector3d &rayDirection, const Eigen::Vector3d &normal)
{
    //For a given incident vector I and surface normal N reflect returns
    //the reflection direction calculated as I - 2.0 * dot(N, I) * N.
    Eigen::Vector3d n = normal.normalized();
    return rayDirection-2*rayDirection.dot(n)*n;
}

/*

Eigen::Vector3d ShaderWidget::getRayCylinderIntersection( const Eigen::Vector3d &rayDirection, const Eigen::Vector3d &rayOrigin, float cylinderRadius )
{
    double a = rayDirection.x()*rayDirection.x()+rayDirection.z()*rayDirection.z();
    double b = 2*(rayDirection.x()*rayOrigin.x()+rayOrigin.z()*rayDirection.z());
    double c = rayOrigin.x()*rayOrigin.x()+rayOrigin.z()*rayOrigin.z()-cylinderRadius*cylinderRadius;
    double t = (-b+sqrt(b*b-4*a*c))/(2*a);

    return rayDirection*t+rayOrigin;
}

Eigen::Vector3d ShaderWidget::getRayPlaneIntersection(const Eigen::Vector3d &rayDirection, const Eigen::Vector3d &rayOrigin, const Eigen::Vector3d &planeNormal, const Eigen::Vector3d &planeOrigin)
{
    double offset = -planeNormal.dot(planeOrigin);
    double t = -(offset+planeNormal.dot(rayOrigin))/ planeNormal.dot(rayDirection);
    return rayDirection*t+rayOrigin;
}
*/
