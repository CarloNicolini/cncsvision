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
#include "Homography.h"
#include "VolumetricHelicoidWidgetALP.h"
#include "VolumetricMeshIntersection.h"
#include "GLText.h"
#include "Timer.h"

#ifdef __APPLE__
std::string baseDir("/Users/rs/cncsvisioncmake/");
#endif

#ifdef __linux__
std::string baseDir("/home/carlo/workspace/cncsvisioncmake/");
#endif

#ifdef WIN32
std::string baseDir("C:/workspace/cncsvisioncmake/");
#endif

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

// This is helpful to understand how to draw off-screen with OpenGL
// http://stackoverflow.com/questions/14785007/can-i-use-opengl-for-off-screen-rendering?rq=1

// For issues about OpenGL inclusion when glew conflicts with OpenGL I suggest to look:
// http://stackoverflow.com/questions/15048729/where-is-glgenbuffers-in-qt5

/**
 * @brief VolumetricHelicoidWidget::VolumetricHelicoidWidget
 * @param parent
 * @param shareWidget
 */
VolumetricHelicoidWidget::VolumetricHelicoidWidget(QWidget *parent) :
    //    QGLWidget(parent)
    QGLWidget( parent)
{
    //cerr << "[VolumetricHelicoidWidget] Constructor" << endl;
    currentGLColor = Qt::gray;
    // Create the timer for the widget
    QTimer *timer = new QTimer(this); timer->start(0);
    QObject::connect(timer,SIGNAL(timeout()),this,SLOT(repaint()));

    drawTextureCube=true;

    isDrawingFrustum=false;
    useCalibratedGLView=false;
    useOffscreenRendering=false;

    this->projectorPixelWidth =PROJECTOR_RESOLUTION_WIDTH;
    this->projectorPixelHeight= PROJECTOR_RESOLUTION_HEIGHT;
    this->slicesNumber=PROJECTOR_SLICES_NUMBER;
    this->resize(projectorPixelWidth,projectorPixelHeight);

    this->eyeZ=-7500.0;
    emit eyeZChanged(eyeZ);

    // Setup the visualization volume
    camCalibration=NULL;
    volume = new VolumetricMeshIntersection(TEXTURE_RESOLUTION_X,TEXTURE_RESOLUTION_Y,TEXTURE_RESOLUTION_Z);
    volume->setUniformColor(glWhite);

    volume->meshStruct.radius=110.0;
    volume->meshStruct.height=1.0;
    volume->meshStruct.rotationAngle=0.0;
    volume->meshStruct.offsetX=0.0;
    volume->meshStruct.offsetY=0.0;
    volume->meshStruct.offsetZ=0.0;

    volume->meshStruct.x=0.0;
    volume->meshStruct.y=0.0;
    volume->meshStruct.z=0.0;

    volume->meshStruct.thickness=0.5f;

    // Setup the calibration camera
    //loadCameraSettings();
    this->setCameraParameters(3.0f,200.0f,30000.0f);
    // Set the current focus in this OpenGL window
    //this->setFocus();

    this->curvature=240.0;

}

/**
 * @brief VolumetricHelicoidWidget::~VolumetricHelicoidWidget
 */
VolumetricHelicoidWidget::~VolumetricHelicoidWidget()
{
    cerr << "[VolumetricHelicoidWidget] Destructor" << endl;
    delete fbo;

    if (volume)
        delete volume;
    if (camCalibration)
        delete camCalibration;
}

/**
 * @brief setCameraParameters
 * @param fieldOfView
 * @param zNear
 * @param zFar
**/
void VolumetricHelicoidWidget::setCameraParameters(double _fieldOfView, double _zNear, double _zFar)
{
    this->FOV=_fieldOfView;
    this->zNear = _zNear;
    this->zFar = _zFar;
    emit zNearChanged(zNear);
    emit zFarChanged(zFar);
    emit fovChanged(FOV);
}

/**
 * @brief VolumetricHelicoidWidget::onZFarChanged
 * @param val
 */
void VolumetricHelicoidWidget::onZFarChanged(double val)
{
    this->zFar = val;
}

/**
 * @brief VolumetricHelicoidWidget::onZNearChanged
 * @param val
 */
void VolumetricHelicoidWidget::onZNearChanged(double val)
{
    this->zNear = val;
}

/**
 * @brief VolumetricHelicoidWidget::setEyeZ
 * @param val
 */
void VolumetricHelicoidWidget::onEyeZChanged(double val)
{
    this->eyeZ=val;
}

/**
 * @brief applyOpenGLCameraFOV
 * @param fieldOfView
 * @param zNear
 * @param zFar
 */
void VolumetricHelicoidWidget::applyOpenGLCameraFOV()
{
#ifdef MONA
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    /*
    cam.setNearFarPlanes(this->zNear,this->zFar);
    cam.setOrthoGraphicProjection(true);
    cam.init(Screen(431.0,272.0,0.0,0.0,eyeZ));
    cam.setEye(Vector3d(0,0,0));
    */
    gluPerspective(this->FOV, (float)this->width() / (float)this->height(), this->zNear,this->zFar);
    //glOrtho(eyeZ,-eyeZ,eyeZ,-eyeZ,0.1,10000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
#endif
    double ratio = (float)this->projectorPixelHeight/(float)this->projectorPixelWidth;
    cam.setOrthoGraphicProjection(true);
    this->cam.init(Screen(500.0,500.0*ratio,0,0,this->eyeZ));
    this->cam.setNearFarPlanes(this->zNear,this->zFar);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    cam.setEye(Vector3d(0,0,0));
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
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
    this->makeCurrent();
    cerr << "[VolumetricHelicoidWidget] initializing GL context" << endl;
    int argc = 1;
    char *argv[] = {"1","2"};
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_INDEX ); // Set up a basic display buffer (only single buffered for now)

    qglClearColor(this->currentGLColor);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set depth buffer clear value
    glClearDepth(1.0);

    // Enable multisample
    //glEnable(GL_MULTISAMPLE);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0,0,-10);
    // Get the informations about the size of texture3D ram
    GLint result;
    glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &result);
    cerr << "[VolumetricHelicoidWidget] Available GPU memory for texture 3D is " << result << " [MB]" << endl;

    // VERY IMPORTANT TO INITIALIZE GLEW BEFORE THE GLSL SHADERS
    glewInit();

    volume->resize(TEXTURE_RESOLUTION_X,TEXTURE_RESOLUTION_Y,TEXTURE_RESOLUTION_Z);
    volume->loadObj(baseDir+"/data/objmodels/helicoid.obj");
    volume->setTexture3DfillValue(0);
    volume->fillVolumeWithSpheres(VOLUME_N_SPHERES,SPHERES_MIN_RADIUS,SPHERES_MAX_RADIUS);
    volume->meshStruct.showMesh=false;
    volume->initializeTexture();

    ////////////////////////////
    const GLcharARB vertexShader[] = STATIC_STRINGIFY(
                uniform float step;
            varying vec3 texture_coordinate;
    uniform float objSize;
    uniform float thickness;
    uniform float curvature;
    varying vec4 pvertex;
    uniform vec3 objOffset;
    void main()
    {
        vec4 v=gl_Vertex;
        // This is to rotate the object
        v.xz = vec2(cos(step)*v.x+sin(step)*v.z,-sin(step)*v.x+cos(step)*v.z);
        // Compute the z position given x and y on a circular domain of radius 1 (diameter 2)
        //texture_coordinate= vec3(((v.xz/radius+1.0)*0.5),(v.y-objOffset.y)/objSize);
        texture_coordinate= vec3(   ((v.xz-objOffset.xz)/objSize+1.0)*0.5, ((v.y-objOffset.y)/objSize+1.0)*0.5);
        gl_Position = gl_ModelViewProjectionMatrix*v;
        pvertex = v;
    }
    );

    const GLcharARB fragmentShader[] = STATIC_STRINGIFY(
                varying vec3 texture_coordinate;
            uniform sampler3D my_color_texture;
    uniform vec4 uniformColor;
    varying vec4 pvertex;
    uniform float thickness;
    uniform float curvature;
    void main()
    {
        vec4 finalColor = uniformColor*texture3D(my_color_texture, texture_coordinate);
        // Filter out the vertices outside a given surface normal
        float parametricSurfaceEquation = (pvertex.x*pvertex.x)/curvature;
        float normalLength = sqrt(1.0+(2.0*pvertex.x/curvature)*(2.0*pvertex.x/curvature));
        if ( abs((pvertex.z - parametricSurfaceEquation)/normalLength) <= thickness)
        {
            gl_FragColor = finalColor;
        }
        else
        {
            // color the vertices outside that small volume around the surface to black
            gl_FragColor = vec4(0.0,0.0,0.0,1.0);
        }
    }
    );

    volume->initializeSurfaceShaders(vertexShader,fragmentShader);
    ////////////////////////////
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPointSize(0.1f);
    glLineWidth(0.1f);
    arcball.setWidthHeight(this->width(),this->height());
    // Initializing frame buffer object
    // Here we create a framebuffer object with the smallest necessary precision, i.e. GL_RED in order to make
    // the subsequent calls to glReadPixels MUCH faster!
    QGLFramebufferObjectFormat fboFormat;
    fboFormat.setMipmap(false);
    fboFormat.setSamples(0);
    fboFormat.setInternalTextureFormat(GL_LUMINANCE);
    //fboFormat.setAttachment(QGLFramebufferObject::Depth); // It makes Win32 version crash
    fbo = new QGLFramebufferObject(QSize(PROJECTOR_RESOLUTION_WIDTH, PROJECTOR_RESOLUTION_HEIGHT),fboFormat);
    cerr << "[VolumetricHelicoidWidget] FBO depth= " << fbo->depth() << endl;
    cerr << "[VolumetricHelicoidWidget] num colors= " << fbo->colorCount() << endl;
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
        glLoadMatrixd(camCalibration->getOpenGLProjectionMatrix().data());
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixd(camCalibration->getOpenGLModelViewMatrix().data());
    }
    else
    {
        applyOpenGLCameraFOV();
    }

    glEnable(GL_TEXTURE_3D);
    volume->meshStruct.shader->begin();
    volume->meshStruct.shader->setUniform4f(static_cast<GLcharARB*>("uniformColor"),1.0f,1.0f,1.0f,1.0f);
    volume->meshStruct.shader->setUniform1f(static_cast<GLcharARB*>("step"),volume->meshStruct.rotationAngle);
    volume->meshStruct.shader->setUniform3f(static_cast<GLcharARB*>("objOffset"),volume->meshStruct.offsetX,volume->meshStruct.offsetY,volume->meshStruct.offsetZ);
    volume->meshStruct.shader->setUniform1f(static_cast<GLcharARB*>("objSize"),volume->meshStruct.radius);
    volume->meshStruct.shader->setUniform1f(static_cast<GLcharARB*>("thickness"),volume->meshStruct.thickness);
    volume->meshStruct.shader->setUniform1f(static_cast<GLcharARB*>("curvature"),this->curvature);

    glPushMatrix();
    glLoadIdentity();
    glTranslated(0,0,this->eyeZ);
    arcball.applyRotationMatrix();
    glRotated(90,1,0,0);
    glTranslated(volume->meshStruct.x,volume->meshStruct.y,volume->meshStruct.z);
    volume->obj->draw();
    glPopMatrix();
    volume->meshStruct.shader->end();
    glDisable(GL_TEXTURE_3D);

    /*
    glPushMatrix();
    glLoadIdentity();
    glTranslated(0,0,this->eyeZ);
    arcball.applyRotationMatrix();
    glRotated(90,1,0,0);
    volume->draw();
    glPopMatrix();
*/
    if ( drawTextureCube )
    {
        glPushMatrix();
        glLoadIdentity();
        glTranslated(0,0,this->eyeZ);
        arcball.applyRotationMatrix();
        glRotated(90,1,0,0);
        glTranslated(volume->meshStruct.offsetX, volume->meshStruct.offsetY, volume->meshStruct.offsetZ);
        glutWireCube(volume->meshStruct.radius*2);
        glPopMatrix();
    }
}

/**
 * @brief VolumetricHelicoidWidget::getVolume
 * @return
 */
VolumetricMeshIntersection *VolumetricHelicoidWidget::getVolume() const
{
    return volume;
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

#ifdef WIN32
#include <windows.h>
size_t getTotalSystemMemory()
{
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    return status.ullTotalPhys;
}
#endif

/**
 * @brief VolumetricHelicoidWidget::startFramesGeneration
 */
void VolumetricHelicoidWidget::generateFrames()
{
    this->drawTextureCube=false;
    this->setHelicoidZeroColor(0);
    this->setCurrentGLColor(Qt::black);
    unsigned long int w = static_cast<unsigned long int>(this->width());
    unsigned long int h = static_cast<unsigned long int>(this->height());
    // Important to use unsigned integers to avoid overflow error
    unsigned long int length = w*h*slicesNumber;
    // Reset space for all frames sequence
    allFrames.clear();
    emit memoryAllocatedMegaBytes(length*sizeof(unsigned char)/1E6);
    //cerr << "[VolumetricHelicoidWidget] w,h,slicesNumber=" << w << "," << h << "," << slicesNumber << endl;
    //cerr << "Resizing to " << w*h*slicesNumber*sizeof(unsigned char)/1E6 << " [MB]" << endl;
    try
    {
        allFrames.resize(length,0);
    }
    catch (const std::exception &e)
    {
        QString maximumVectorSize = QString::number(allFrames.max_size()/1E6);
        QMessageBox::warning(this,QString("Error: ")+QString::fromStdString(e.what()),"Too much memory allocated, asked "+QString::number(length/1E6) + " [MB] but " + maximumVectorSize + " [MB] addressable" + "\n"+"User memory= "+QString::number((unsigned int)getTotalSystemMemory()/1E6)+" [MB]");
        return;
    }

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    volume->meshStruct.rotationAngle=0;
    Timer timer; timer.start();
    double deltaAngle = (2.0*M_PI)/this->slicesNumber;
    // Start the loop of frames generation
    if ( !useOffscreenRendering )
    {
        for ( unsigned int i=0; i<slicesNumber; i++ )
        {
            int percentage = static_cast<int>(std::ceil(100.0/slicesNumber*(i+1)));
            emit framePercentageGenerated(percentage);
            this->repaint();
            volume->meshStruct.rotationAngle += deltaAngle;
        }
    }
    else
    {
        fbo->bind();
        for ( unsigned int i=0; i<slicesNumber; i++ )
        {
            this->paintGL();
            // Read the current frame buffer object
            glReadPixels(0, 0, w, h, GL_LUMINANCE, GL_UNSIGNED_BYTE, &allFrames.at(i*w*h));
            //fbo->toImage().save(QDir::currentPath()+"/ciccio_"+QString::number(i,'g',3)+".png","PNG");
            volume->meshStruct.rotationAngle += deltaAngle;
        }
        fbo->release();
    }
    getGLerrors();

    cerr << "[VolumetricHelicoidWidget] " << slicesNumber <<" frames generated, [ms]/frame = " << timer.getElapsedTimeInMilliSec()/slicesNumber << endl;
    this->setHelicoidZeroColor(volume->getTexture3DfillValue());
    glPopAttrib();

    this->drawTextureCube=true;
    this->setCurrentGLColor(Qt::gray);
    this->repaint();
}

/**
 * @brief VolumetricHelicoidWidget::resizeGL
 * @param width
 * @param height
 */
void VolumetricHelicoidWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, (GLsizei) width, (GLsizei) height); // Set our viewport to the size of our window
    applyOpenGLCameraFOV();
    arcball.setWidthHeight(this->width(),this->height());
}

/**
 * @brief VolumetricHelicoidWidget::wheelEvent
 * @param e
 */
void VolumetricHelicoidWidget::wheelEvent(QWheelEvent *e)
{
    eyeZ+=e->delta()*0.1*eyeZ/100.0;
    emit eyeZChanged(eyeZ);
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
    case Qt::Key_Plus:
    {
        volume->meshStruct.rotationAngle+=(2.0*M_PI)/this->slicesNumber;
        break;
    }
    case Qt::Key_Minus:
    {
        volume->meshStruct.rotationAngle-=(2.0*M_PI)/this->slicesNumber;
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
 * @brief VolumetricHelicoidWidget::setOffset
 * @param x
 * @param y
 * @param z
 */
void VolumetricHelicoidWidget::setObjectOffset(double x, double y, double z)
{
    this->volume->meshStruct.offsetX=x;
    this->volume->meshStruct.offsetY=y;
    this->volume->meshStruct.offsetZ=z;
}

/**
 * @brief VolumetricHelicoidWidget::setHelicoidOffset
 * @param x
 * @param y
 * @param z
 */
void VolumetricHelicoidWidget::setHelicoidOffset(double x, double y, double z)
{
    this->volume->meshStruct.x = x;
    this->volume->meshStruct.y = y;
    this->volume->meshStruct.z = z;
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
 * @brief VolumetricHelicoidWidget::setSlicesNumber
 * @param nSlices
 */
void VolumetricHelicoidWidget::setSlicesNumber(int nSlices)
{
    // This avoids integer overflow when resizing vector!
    this->slicesNumber=static_cast<unsigned int>(nSlices);
}

/**
 * @brief VolumetricHelicoidWidget::randomizeSpheres
 * @param nSpheres
 * @param minRadius
 * @param maxRadius
 */
void VolumetricHelicoidWidget::randomizeSpheres(bool useRandomDots, int nSpheres, int minRadius, int maxRadius)
{

    volume->setTexture3DfillValue(0);
    if (useRandomDots)
        volume->fillVolumeWithRandomDots(nSpheres,minRadius);
    else
        volume->fillVolumeWithSpheres(nSpheres,minRadius,maxRadius);

    volume->meshStruct.showMesh=false;
    volume->initializeTexture();
}

/**
 * @brief VolumetricHelicoidWidget::loadBinvox
 * @param filename
 */
void VolumetricHelicoidWidget::loadBinvox(const string &filename)
{
    volume->setTexture3DfillValue(0);
    volume->loadTexture3DFile(filename);
    QString informations("Texture size ");
    informations += "[ "+QString::number(this->volume->getTextureSizeX())+","+QString::number(this->volume->getTextureSizeY())+","+QString::number(this->volume->getTextureSizeZ())+ "]";
    emit binVoxLoaded(informations);
    volume->initializeTexture();
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

/**
 * @brief VolumetricHelicoidWidget::setOffscreenRendering
 * @param val
 */
void VolumetricHelicoidWidget::setOffscreenRendering(bool val)
{
    this->useOffscreenRendering = val;
}


void VolumetricHelicoidWidget::onSurfaceThicknessChanged(double val)
{
    this->update();
    this->volume->meshStruct.thickness=val;
}

void VolumetricHelicoidWidget::onSurfaceCurvatureChanged(double val)
{
    this->curvature=val;
}

/**
 * @brief VolumetricHelicoidWidget::computeCameraCalibrationMatrices
 * @param points2d
 * @param points3d
 */
void VolumetricHelicoidWidget::computeCameraCalibrationMatrices(const QString &points2Dfilename,const QString &points3Dfilename)
{
    // First load the 2D and 3D points
    /*
#ifdef __linux__
    QString points2Dfilename = "/home/carlo/Desktop/3D-Display/Calibrations/2D_points.txt";
    QString points3Dfilename = "/home/carlo/Desktop/3D-Display/Calibrations/3D_points.txt";
#endif
#ifdef _WIN32
    QString points2Dfilename = "I:/Images/22July2013/2D_points.txt";
    QString points3Dfilename = "I:/Images/22July2013/3D_points.txt";
#endif
*/
    int width = PROJECTOR_RESOLUTION_WIDTH;
    int height = PROJECTOR_RESOLUTION_HEIGHT;
    bool decomposePMatrix = true;
    bool computeOpenGLMatrices = true;
    camCalibration = new CameraDirectLinearTransformation(points2Dfilename.toStdString(),points3Dfilename.toStdString(),decomposePMatrix,computeOpenGLMatrices,0,0,width,height,this->zNear,this->zFar);

    this->Projection = camCalibration->getOpenGLProjectionMatrix().matrix();
    this->ModelView = camCalibration->getOpenGLModelViewMatrix().matrix();

    cout << "HZ 3x4 projection matrix=\n" << camCalibration->getProjectionMatrix() << endl;
    cout << "Intrinsinc camera matrix=\n" <<camCalibration->getIntrinsicMatrix() << endl;
    cout << "Extrinsic camera matrix=\n"<< camCalibration->getRotationMatrix() << endl << endl;
    cout << "Camera Center C=" << camCalibration->getCameraPositionWorld().transpose() << endl;
    cout << "Camera t= " << camCalibration->getT().transpose() << endl;
    cout << "Camera Principal axis= " << camCalibration->getPrincipalAxis().transpose() << endl;
    cout << "Camera Principal point=" << camCalibration->getPrincipalPoint().transpose() << endl ;
    cout << "OpenGL ModelViewMatrix=\n" << camCalibration->getOpenGLModelViewMatrix().matrix() << endl;
    cout << "OpenGL Projection=\n" << camCalibration->getOpenGLProjectionMatrix().matrix() << endl;
    cout << "Reproduction error= " << camCalibration->getReprojectionError(camCalibration->getProjectionMatrix(),camCalibration->points2D,camCalibration->points3D) << endl;
}
