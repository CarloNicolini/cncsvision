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

// Only include GLEW in this way in order to ensure compatibility with QT OpenGL widget
#include <GL/glew.h>
#include "VolumetricHelicoidWidget.h"
#include "Homography.h"
#include "VolumetricMeshIntersection.h"
#include "GLText.h"

#ifdef __APPLE__
std::string CNCSVISION_BASE_DIRECTORY("/Users/rs/cncsvisioncmake/");
#endif

#ifdef __linux__
std::string CNCSVISION_BASE_DIRECTORY("/home/carlo/workspace/cncsvisioncmake/");
#endif

#ifdef WIN32
std::string CNCSVISION_BASE_DIRECTORY("C:/workspace/cncsvisioncmake/");
#endif

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

// This is helpful to understand how to draw off-screen with OpenGL
// http://stackoverflow.com/questions/14785007/can-i-use-opengl-for-off-screen-rendering?rq=1

// For issues about OpenGL inclusion when glew conflicts with OpenGL I suggest to look:
// http://stackoverflow.com/questions/15048729/where-is-glgenbuffers-in-qt5

/**
 * @brief setGLPerspective
 * @param width
 * @param height
 */
void setGLPerspective(int width, int height)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float FOV=14.0;
    double zNear=10.0;
    double zFar = 1500.0;
    gluPerspective(FOV, (float)width / (float)height, zNear,zFar);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/**
 * @brief VolumetricHelicoidWidget::VolumetricHelicoidWidget
 * @param parent
 * @param shareWidget
 */
VolumetricHelicoidWidget::VolumetricHelicoidWidget(QWidget *parent) :
    //    QGLWidget(parent)
    QGLWidget( parent)
{
    currentGLColor = Qt::gray;
    // Create the timer for the widget
    QTimer *timer = new QTimer(this); timer->start(15);
    QObject::connect(timer,SIGNAL(timeout()),this,SLOT(repaint()));

    volume=NULL;
    //Zero the number of frames
    viewCount=0;
    frameNumber=0;
    drawMesh=false;
    drawText=false;
    isSavingFrames=false;
    isDrawingFrustum=false;
    useCalibratedGLView=false;

    this->projectorPixelWidth =PROJECTOR_RESOLUTION_WIDTH;
    this->projectorPixelHeight= PROJECTOR_RESOLUTION_HEIGHT;
    this->viewsNumber=PROJECTOR_SLICES_NUMBER;
    this->resize(projectorPixelWidth,projectorPixelHeight);

    this->eyeZ=-500.0;

    // Setup the visualization volume
    cam=NULL;
    volume = new VolumetricMeshIntersection(TEXTURE_RESOLUTION_X,TEXTURE_RESOLUTION_Y,TEXTURE_RESOLUTION_Z);
    volume->setUniformColor(glWhite);

    volume->meshStruct.radius=120.0;
    volume->meshStruct.height=1.0;
    volume->meshStruct.rotationAngle=0.0;
    volume->meshStruct.offsetX=0.0;
    volume->meshStruct.offsetY=0.0;
    volume->meshStruct.offsetZ=0.0;

    volume->fillVolumeWithSpheres(1000,5,50);

    // Setup the calibration camera
    //loadCameraSettings();

    // Set the current focus in this OpenGL window
    this->setFocus();
}

/**
 * @brief VolumetricHelicoidWidget::setOutputFolderName
 * @param name
 */
void VolumetricHelicoidWidget::setOutputFolderName(const QString &name)
{
    this->folderName = name;
}

/**
 * @brief VolumetricHelicoidWidget::loadSettings
 */
void VolumetricHelicoidWidget::loadCameraSettings()
{
    // First load the 2D and 3D points
#ifdef __linux__
    QString points2Dfilename = "/home/carlo/Desktop/3D-Display/Calibrations/2D_points.txt";
    QString points3Dfilename = "/home/carlo/Desktop/3D-Display/Calibrations/3D_points.txt";
#endif
#ifdef _WIN32
    QString points2Dfilename = "I:/Images/22July2013/2D_points.txt";
    QString points3Dfilename = "I:/Images/22July2013/3D_points.txt";
#endif
    double zNear = 1.0;
    double zFar = 1500.0;
    int width = PROJECTOR_RESOLUTION_WIDTH;
    int height = PROJECTOR_RESOLUTION_HEIGHT;
    bool decomposePMatrix = true;
    bool computeOpenGLMatrices = true;
    cam = new CameraDirectLinearTransformation(points2Dfilename.toStdString(),points3Dfilename.toStdString(),decomposePMatrix,computeOpenGLMatrices,0,0,width,height,zNear,zFar);

    this->Projection = cam->getOpenGLProjectionMatrix().matrix();
    this->ModelView = cam->getOpenGLModelViewMatrix().matrix();

    cout << "HZ 3x4 projection matrix=\n" << cam->getProjectionMatrix() << endl;
    cout << "Intrinsinc camera matrix=\n" <<cam->getIntrinsicMatrix() << endl;
    cout << "Extrinsic camera matrix=\n"<< cam->getRotationMatrix() << endl << endl;
    cout << "Camera Center C=" << cam->getCameraPositionWorld().transpose() << endl;
    cout << "Camera t= " << cam->getT().transpose() << endl;
    cout << "Camera Principal axis= " << cam->getPrincipalAxis().transpose() << endl;
    cout << "Camera Principal point=" << cam->getPrincipalPoint().transpose() << endl ;
    cout << "OpenGL ModelViewMatrix=\n" << cam->getOpenGLModelViewMatrix().matrix() << endl;
    cout << "OpenGL Projection=\n" << cam->getOpenGLProjectionMatrix().matrix() << endl;
    cout << "Reproduction error= " << cam->getReprojectionError(cam->getProjectionMatrix(),cam->points2D,cam->points3D) << endl;

}

/**
 * @brief VolumetricHelicoidWidget::~VolumetricHelicoidWidget
 */
VolumetricHelicoidWidget::~VolumetricHelicoidWidget()
{
    if (volume)
        delete volume;
    if (cam)
        delete cam;
}

/**
 * @brief VolumetricHelicoidWidget::minimumSizeHint
 * @return
 */
QSize VolumetricHelicoidWidget::minimumSizeHint() const
{
    return QSize(PROJECTOR_RESOLUTION_WIDTH,PROJECTOR_RESOLUTION_HEIGHT);
}

/**
 * @brief VolumetricHelicoidWidget::sizeHint
 * @return
 */
QSize VolumetricHelicoidWidget::sizeHint() const
{
    return QSize(PROJECTOR_RESOLUTION_WIDTH,PROJECTOR_RESOLUTION_HEIGHT);
}

/**
 * @brief VolumetricHelicoidWidget::initializeGL
 */
void VolumetricHelicoidWidget::initializeGL()
{
    int argc = 1;
    char *argv[] = {""};

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE ); // Set up a basic display buffer (only single buffered for now)

    qglClearColor(this->currentGLColor);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set depth buffer clear value
    glClearDepth(1.0);

    // Enable multisample
    glEnable(GL_MULTISAMPLE);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0,0,-10);
    // Get the informations about the size of texture3D ram
    GLint result;
    glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &result);
    cerr << "Info: Available memory is " << result << " [bytes]" << endl;

    // VERY IMPORTANT TO INITIALIZE GLEW BEFORE THE GLSL SHADERS
    glewInit();

    this->makeCurrent();

    volume->resize(TEXTURE_RESOLUTION_X,TEXTURE_RESOLUTION_Y,TEXTURE_RESOLUTION_Z);
    volume->loadObj(CNCSVISION_BASE_DIRECTORY+"/data/objmodels/helicoid.obj");
    volume->setTexture3DfillValue(0);
    volume->fillVolumeWithSpheres(1000,5,25);
    volume->meshStruct.showMesh=false;
    volume->initializeTexture();
    volume->initializeSurfaceShaders();

    glPointSize(0.1f);
    glLineWidth(0.1f);

    arcball.setWidthHeight(this->width(),this->height());
    getGLerrors();
}

/**
 * @brief VolumetricHelicoidWidget::draw
 */
void VolumetricHelicoidWidget::draw()
{
    if ( useCalibratedGLView )
    {
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixd(cam->getOpenGLProjectionMatrix().data());
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixd(cam->getOpenGLModelViewMatrix().data());
    }
    else
    {
        setGLPerspective(this->width(),this->height());
    }

    if ( isDrawingFrustum )
    {
        glPushAttrib(GL_COLOR_BUFFER_BIT);
        glPushMatrix();
        glLoadIdentity();
        glTranslated(0.0,0.0,eyeZ);
        arcball.applyRotationMatrix();
        glColor3d(1.0,1.0,1.0);
        if (useCalibratedGLView)
        {
            drawFrustum(cam->getOpenGLProjectionMatrix().data(),&cam->getOpenGLModelViewMatrix().inverse().matrix()(0,0));
        }
        else
        {
            Eigen::Matrix4d P,MV,iMV;
            glGetDoublev(GL_MODELVIEW_MATRIX,MV.data());
            glGetDoublev(GL_PROJECTION_MATRIX,P.data());
            iMV = MV.inverse();
            drawFrustum(P.data(),iMV.data());
        }
        glPopMatrix();
        glPopAttrib();
    }

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
    glRotated(90,1,0,0);
    glTranslated(volume->meshStruct.offsetX, volume->meshStruct.offsetY, volume->meshStruct.offsetZ);
    if (!isSavingFrames)
    glutWireCube(volume->meshStruct.radius*2);
    glPopMatrix();

    /*

    if ( isDrawingFrustum)
    {
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
        glRotated(90,1,0,0);
        glTranslated(volume->meshStruct.offsetX, volume->meshStruct.offsetY, volume->meshStruct.offsetZ);
        glutWireCube(volume->meshStruct.radius*2);
        glPopMatrix();
    }
    else
    {
        drawVisualizedCameraScene();
    }
    */
}

/**
 * @brief VolumetricHelicoidWidget::myDraw
 */
void VolumetricHelicoidWidget::myDraw()
{

}

/**
 * @brief VolumetricHelicoidWidget::drawVisualizedCameraScene
 */
void VolumetricHelicoidWidget::drawVisualizedCameraScene()
{
    // if (points2Dfilename.isEmpty() || points3Dfilename.isEmpty() )
    // return;


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

/**
 * @brief VolumetricHelicoidWidget::paintGL
 */
void VolumetricHelicoidWidget::paintGL()
{
    qglClearColor(this->currentGLColor);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    draw();
/*
    if (isSavingFrames)
        saveFrame(this->folderName);
*/
    //volume->meshStruct.rotationAngle += (2.0*M_PI)/this->viewsNumber;
    ////////////////////////////////////////
}

/**
 * @brief VolumetricHelicoidWidget::saveFrame
 */
void VolumetricHelicoidWidget::saveFrame(const QString &outputDirectory)
{
    QString frameString;
    frameString.sprintf("%04d", frameNumber);
    frameNumber++;
    QString outputFrameFileName = outputDirectory + QDir::separator()+ QString(QDir::separator())+ QString("img_") + frameString +".bmp";

    QImage frame = this->grabFrameBuffer();

    int dpm = ceil(72.0 / 0.0254); // ~300 DPI
    frame.setDotsPerMeterX(dpm);
    frame.setDotsPerMeterY(dpm);
    frame = frame.convertToFormat(QImage::Format_Mono);
    bool saveALP=true;
    if (saveALP)
    {
        for (int i=0; i<frame.width()*frame.height(); i++)
            allFrames.push_back(*(frame.bits()+i));
        //frame.pixelIndex()
    }
    if (  !frame.save( outputFrameFileName, NULL, -1) )
    {
        QMessageBox::warning(this,"Warning saving image","Can't save to file "+outputFrameFileName);
    }
}

/**
 * @brief VolumetricHelicoidWidget::setCurrentGLColor
 * @param val
 */
void VolumetricHelicoidWidget::setCurrentGLColor(Qt::GlobalColor val)
{
    this->currentGLColor=val;
}

/**
 * @brief VolumetricHelicoidWidget::setHelicoidZeroColor
 * @param value
 */
void VolumetricHelicoidWidget::setHelicoidZeroColor(int value)
{
    volume->setTexture3DfillValue(value);
}

/**
 * @brief VolumetricHelicoidWidget::startFramesGeneration
 */
void VolumetricHelicoidWidget::startFramesGeneration()
{
    // Reset space for all frames sequence
    allFrames.resize(this->width()*this->height()*this->viewsNumber,0);

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    volume->meshStruct.rotationAngle=0;
    this->setHelicoidZeroColor(0);
    isSavingFrames=true;
    this->frameNumber=0;
    for ( int i=0; i<viewsNumber; i++ )
    {
        this->repaint();
        QImage frame = this->grabFrameBuffer().convertToFormat(QImage::Format_Mono);;
        bool saveALP=true;
        if (saveALP)
        {
            for (int i=0; i<frame.width()*frame.height(); i++)
                allFrames.push_back(*(frame.bits()+i));
        }
        volume->meshStruct.rotationAngle += (2.0*M_PI)/this->viewsNumber;
    }

    cerr << "Size of allframes= " << allFrames.size() << endl;

    isSavingFrames=false;
    this->setHelicoidZeroColor(volume->getTexture3DfillValue());
    this->repaint();
    glPopAttrib();
}

/**
 * @brief VolumetricHelicoidWidget::resizeGL
 * @param width
 * @param height
 */
void VolumetricHelicoidWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, (GLsizei) width, (GLsizei) height); // Set our viewport to the size of our window
    setGLPerspective(width,height);
    arcball.setWidthHeight(this->width(),this->height());
}

/**
 * @brief VolumetricHelicoidWidget::wheelEvent
 * @param e
 */
void VolumetricHelicoidWidget::wheelEvent(QWheelEvent *e)
{
    eyeZ+=e->delta()*1E-1;
}

/**
 * @brief VolumetricHelicoidWidget::keyPressEvent
 * @param e
 */
void VolumetricHelicoidWidget::keyPressEvent(QKeyEvent *e)
{
    switch ( e->key() )
    {
    case Qt::Key_Q:
    {
        QApplication::exit(0);
        break;
    }
    case Qt::Key_A:
    {
        volume->meshStruct.rotationAngle+=(2.0*M_PI)/this->viewsNumber;
        break;
    }
    case Qt::Key_Z:
    {
        volume->meshStruct.rotationAngle-=(2.0*M_PI)/this->viewsNumber;
        break;
    }
    case Qt::Key_7:
    {
        volume->meshStruct.offsetZ+=2.0;
        break;
    }
    case Qt::Key_9:
    {
        volume->meshStruct.offsetZ-=2.0;
        break;
    }
    case Qt::Key_2:
    {
        volume->meshStruct.offsetY-=2.0;
        break;
    }
    case Qt::Key_8:
    {
        volume->meshStruct.offsetY+=2.0;
        break;
    }
    case Qt::Key_4:
    {
        volume->meshStruct.offsetX-=2.0;
        break;
    }
    case Qt::Key_6:
    {
        volume->meshStruct.offsetX+=2.0;
        break;
    }
    case Qt::Key_W:
    {
        viewCount++;
        break;
    }
    case Qt::Key_S:
    {
        viewCount--;

        break;
    }
    case Qt::Key_M:
    {
        drawMesh=!drawMesh;
        break;
    }
    case Qt::Key_T:
    {
        drawText=!drawText;
        break;
    }
    }
}

/**
 * @brief VolumetricHelicoidWidget::mousePressEvent
 * @param event
 */
void VolumetricHelicoidWidget::mousePressEvent(QMouseEvent *event)
{
    this->setFocus();
    if (event->button() == Qt::LeftButton)
    {
        arcball.startRotation(event->x(),event->y());
    }
}

/**
 * @brief VolumetricHelicoidWidget::mouseDoubleClickEvent
 */

void VolumetricHelicoidWidget::mouseDoubleClickEvent(QMouseEvent *)
{
    arcball.reset();
}

/**
 * @brief VolumetricHelicoidWidget::mouseReleaseEvent
 * @param event
 */
void VolumetricHelicoidWidget::mouseReleaseEvent(QMouseEvent *event)
{
    arcball.stopRotation();
}

/**
 * @brief VolumetricHelicoidWidget::mouseMoveEvent
 * @param event
 */
void VolumetricHelicoidWidget::mouseMoveEvent(QMouseEvent *event)
{
    arcball.updateRotation(event->x(),event->y());
}

/**
 * @brief VolumetricHelicoidWidget::getVolume
 * @return
 */
void VolumetricHelicoidWidget::setOffset(double x, double y, double z)
{
    this->volume->meshStruct.offsetX=x;
    this->volume->meshStruct.offsetY=y;
    this->volume->meshStruct.offsetZ=z;
}

/**
 * @brief VolumetricHelicoidWidget::setObjectScale
 * @param objScale
 */
void VolumetricHelicoidWidget::setObjectScale(double objScale)
{
    this->volume->meshStruct.radius=objScale;
}

/**
 * @brief VolumetricHelicoidWidget::setRotation
 * @param angle
 */
void VolumetricHelicoidWidget::setRotation(double angle)
{
    this->volume->meshStruct.rotationAngle=angle;
}

/**
 * @brief VolumetricHelicoidWidget::setSlicesNumber
 * @param nSlices
 */
void VolumetricHelicoidWidget::setSlicesNumber(int nSlices)
{
    this->viewsNumber=nSlices;
}

/**
 * @brief VolumetricHelicoidWidget::randomizeSpheres
 * @param nSpheres
 * @param minRadius
 * @param maxRadius
 */
void VolumetricHelicoidWidget::randomizeSpheres(int nSpheres, int minRadius, int maxRadius)
{
    volume->setTexture3DfillValue(0);
    volume->fillVolumeWithSpheres(nSpheres,minRadius,maxRadius);
    volume->meshStruct.showMesh=false;
    volume->initializeTexture();
}

/**
 * @brief VolumetricHelicoidWidget::setPoints2Dfilename
 * @param val
 */
void VolumetricHelicoidWidget::setPoints2Dfilename(const QString &val)
{
    this->points2Dfilename=val;
}

/**
 * @brief VolumetricHelicoidWidget::setPoints3Dfilename
 * @param val
 */
void VolumetricHelicoidWidget::setPoints3Dfilename(const QString &val)
{
    this->points3Dfilename=val;
}

/**
 * @brief VolumetricHelicoidWidget::toggleStandardGL
 * @param val
 */
void VolumetricHelicoidWidget::toggleStandardGL(bool val)
{
    isDrawingFrustum=!isDrawingFrustum;
    this->arcball.reset();
}

/**
 * @brief VolumetricHelicoidWidget::toggleUseCalibratedGLView
 */
void VolumetricHelicoidWidget::toggleUseCalibratedGLView()
{
    useCalibratedGLView=!useCalibratedGLView;
    this->arcball.reset();
}
