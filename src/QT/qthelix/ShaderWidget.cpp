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
#include "GLUtils.h"
#include "GLText.h"
#define GL_FRAGMENT_PRECISION_HIGH 1

#ifdef __APPLE__
QString CNCSVISION_BASE_DIRECTORY("/Users/rs/cncsvisioncmake/");
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
    //    QGLWidget(parent)
    QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    obj = new ObjLoader2();
    shaderProgram=NULL;
    helicoidRotationAngle=0.0f;
    viewCount=0;
    drawMesh=false;
    drawText=false;

    /*
    glDisable(GL_MULTISAMPLE);
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glDisable(GL_SMOOTH);
    glDisable(GL_LINE_SMOOTH);
*/
    // First load the 2D and 3D points
    //QMessageBox::information(0, "2D points selection", "Please select the 2D points file for the calibration. The points file must contain just two columns of pixel coordinates.");
#ifdef __linux__
    QString points2Dfilename = "/home/carlo/Desktop/JORDANKA/Calibrations/2D_points.txt";
#else
    //QMessageBox::information(this, "3D points selection", "Please select the corresponding 3D points file for the calibration. The points file must contain just three columns of world coordinates.\nThe number of 3D points must be the same of the 2D points.");
    QString points2Dfilename = "I:/Images/22July2013/2D_points.txt";// = QFileDialog::getOpenFileName(this,"Select 2D projected points correspondence", "I:/Images/","*.txt");
#endif

    if (points2Dfilename.isEmpty())
    {
        QMessageBox::critical(this,"Error","Select a valid (non-empty) file for 2D points. Now exiting...");
        QCoreApplication::exit(-1);
    }
    QFileInfo fileInfo2dpoints(points2Dfilename);
	
#ifdef __linux__
    QString points3Dfilename = "/home/carlo/Desktop/JORDANKA/Calibrations/3D_points.txt";
#else
    QString points3Dfilename = "I:/Images/22July2013/3D_points.txt";  // = QFileDialog::getOpenFileName(this,"Select 3D projected points correspondence", fileInfo2dpoints.absolutePath(),"*.txt");
#endif

    if ( points3Dfilename.isEmpty() )
    {
        QMessageBox::critical(this,"Error","Select a valid (non-empty)  file for 3D points. Now exiting...");
        QCoreApplication::exit(-1);
    }

#ifdef __linux__
    QString outputFileName = "/home/carlo/Desktop/JORDANKA/Calibrations/imgs/img";
#else
    QString outputFileName = "I:/Images/02August2013/imgs/cylinder/img";
    //QMessageBox::information(this, "Output images", "Please select the output path for the generated images");  // = QFileDialog::getSaveFileName(NULL,"Select output images path",fileInfo2dpoints.absolutePath());
#endif
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
    CameraDirectLinearTransformation cam(points2Dfilename.toStdString(),points3Dfilename.toStdString(),decomposePMatrix,computeOpenGLMatrices,0,0,width,height,zNear,zFar);

    this->Projection.resize(4,4);
    this->ModelView.resize(4,4);
    this->Projection << cam.getOpenGLProjectionMatrix().matrix();
    this->ModelView << cam.getOpenGLModelViewMatrix().matrix();

    cerr << "CameraPrincipalAxis=" << cam.getPrincipalAxis().transpose() << "\n" << "CameraPrincipalPoint=" <<cam.getPrincipalPoint().transpose() << endl;
    cerr << "CameraProjectionMatrix=\n" << cam.getOpenGLProjectionMatrix().matrix() << endl;
    cerr << "CameraModelViewMatrix=\n" << cam.getOpenGLModelViewMatrix().matrix() << endl;
#ifdef __linux__
    QString objFileName = "/home/carlo/workspace/cncsvisioncmake/data/objmodels/cylinder2.obj";
#else
    //QMessageBox::information(this, "Wavefront Object loading", "Please select the 3D mesh in Wavefront OBJ format to be loaded.");
    QString objFileName = "C:/workspace/cncsvisioncmake/data/objmodels/iit3Drotated.obj";  // =QFileDialog::getOpenFileName(this,"Select Wavefront OBJ mesh file",QDir::currentPath(),"*.obj");
#endif

    if (objFileName.isEmpty())
    {
        QMessageBox::critical(this,"Error","Select a valid (non-empty) file name for OBJ model to load.");
        QCoreApplication::exit(-1);
    }
    this->loadObjFile(objFileName.toStdString());
#ifdef __linux__
    QString projectorParametersFileName = "/home/carlo/Desktop/JORDANKA/Calibrations/ProjectorParameters.txt";
#else
    //QMessageBox::information(this, "Wavefront Object loading", "Please select the projector parameters file.");
    QString projectorParametersFileName = "I:/Images/22July2013/ProjectoParameters.txt";    // =QFileDialog::getOpenFileName(this,"Select projector parameters file name",fileInfo2dpoints.absolutePath(),"*.txt");
#endif
    if (projectorParametersFileName.isEmpty())
    {
        QCoreApplication::exit(-1);
    }
    this->projectorParameters.loadParameterFile(projectorParametersFileName.toStdString());

    // Typical parameters are
    // ProjectorAreaWidth 604
    // ProjectorAreaHeight 684
    // SlicesNumber: 96
    this->projectorPixelWidth =604;
    this->projectorPixelHeight= 608;
    this->viewsNumber=96;
    this->objectScale=120.0f;
    eyeZ=-300.0;
    objectTranslation << objectScale/3*2,0,0;

    /*
    try
    {
        this->viewsNumber= util::str2num<int>(this->projectorParameters.find("ViewsNumber") );
        this->objectScale= util::str2num<double>(this->projectorParameters.find("ObjectScale") );
    }
    catch ( std::invalid_argument &e )
    {
        cerr << e.what() << endl;
        QMessageBox::critical(this,"Parameters files error ",e.what());
        QCoreApplication::exit(-1);
    }
    */

    this->resize(projectorPixelWidth,projectorPixelHeight);
    glPointSize(0.1f);
    glLineWidth(0.1f);
    QTimer *timer = new QTimer(this);
    timer->start(15);
    QObject::connect(timer,SIGNAL(timeout()),this,SLOT(repaint()));

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
            //obj->setFillColor(0.0,0.0,0.0,1.0);
            //obj->setLineColor(1.0,1.0,1.0,1.0);
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
#ifdef __linux__
    QString geometryShaderFileName="/home/carlo/workspace/cncsvisioncmake/src/QT/qthelix/HelicoidIntersectionShader.geom";
    QString vertexShaderFileName="/home/carlo/workspace/cncsvisioncmake/src/QT/qthelix/HelicoidIntersectionShader.vert";
    QString fragmentShaderFileName="/home/carlo/workspace/cncsvisioncmake/src/QT/qthelix/HelicoidIntersectionShader.frag";
#else
	QString geometryShaderFileName="C:/workspace/cncsvisioncmake/src/QT/qthelix/HelicoidIntersectionShader.geom";
    QString vertexShaderFileName="C:/workspace/cncsvisioncmake/src/QT/qthelix/HelicoidIntersectionShader.vert";
    QString fragmentShaderFileName="C:/workspace/cncsvisioncmake/src/QT/qthelix/HelicoidIntersectionShader.frag";
#endif
    if ( shaderProgram!=NULL )
        delete shaderProgram;

    // Create the new shader programs
    this->makeCurrent();
    this->shaderProgram = new QGLShaderProgram(this);

    this->shaderProgram->addShaderFromSourceFile(QGLShader::Vertex,vertexShaderFileName);
    this->shaderProgram->addShaderFromSourceFile(QGLShader::Fragment,fragmentShaderFileName);
    this->shaderProgram->addShaderFromSourceFile(QGLShader::Geometry,geometryShaderFileName);
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
    getGLerrors();
    qDebug()<< "OBJ initialized " ;
}

/**
 * @brief convertEigenMatrixToQt
 * @param M
 * @return
 */
QMatrix4x4 convertEigenMatrix4dToQMatrix4x4(const Eigen::Matrix<GLfloat,4,4> &M)
{
    QMatrix4x4 QM;
    for (int i=0; i<4;i++)
    {
        for (int j=0; j<4; j++)
        {
            QM(i,j)=M(i,j);
        }
    }
    return QM;
}

/**
 * @brief ShaderWidget::drawGpuComputedM
 */
Eigen::Matrix<GLfloat,4,4> getMVPMatrixFromGL(const AffineGL3f &viewTransformation)
{
    AffineGL3f currentGLModelViewMatrix;
    ProjectiveGL3f currentGLProjectionMatrix;

    ProjectiveGL3f eMVP;
    glGetFloatv(GL_MODELVIEW_MATRIX,currentGLModelViewMatrix.data());
    glGetFloatv(GL_PROJECTION_MATRIX,currentGLProjectionMatrix.data());
    currentGLModelViewMatrix.matrix()*=viewTransformation.matrix();
    eMVP=currentGLProjectionMatrix*currentGLModelViewMatrix;

    return eMVP.matrix();
}

/**
 * @brief ShaderWidget::draw
 */
void ShaderWidget::draw()
{
    // Create the object affine transformation directly for the vertices, the vertices after the ObjectA
    // matrix affine transformation are fed to the geometry shader
    AffineGL3f ObjectA = AffineGL3f::Identity();
    ObjectA.linear().matrix().diagonal()*=objectScale;
    ObjectA.translation() << objectTranslation;
    this->shaderProgram->link();
    this->shaderProgram->bind();

    ////// DRAW THE ACTUAL VERTICES //////
    glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    //glLoadMatrixd(Projection.data());
    glMatrixMode(GL_MODELVIEW);
    //glLoadMatrixd(ModelView.data());
    this->shaderProgram->setUniformValue("objectTransformation",convertEigenMatrix4dToQMatrix4x4(ObjectA.matrix()));
    AffineGL3f A=AffineGL3f::Identity();
    A.linear().matrix() << AngleAxisGLf(M_PI/2,VectorGL3f::UnitX()).toRotationMatrix();
    A.translation() << 0,0,eyeZ;
    QMatrix4x4 MVP = convertEigenMatrix4dToQMatrix4x4(getMVPMatrixFromGL(A));

    // Now begins the shader with only the vertices
    this->shaderProgram->setUniformValue("MVP",MVP);
    this->shaderProgram->setUniformValue("color",1.0f,1.0f,1.0f,1.0f);
    this->shaderProgram->setUniformValue("useIntersectionShader",true);
    this->shaderProgram->setUniformValue("helicoidTheta",helicoidRotationAngle);
    this->shaderProgram->setUniformValue("helicoidRadius",120.0f);
    this->shaderProgram->setUniformValue("helicoidHeight",120.0f);
    obj->draw(GL_LINE);
    if (drawMesh)
    {
        glLineWidth(0.01f);
        this->shaderProgram->setUniformValue("color",0.2f,0.2f,0.2f,1.0f);
        this->shaderProgram->setUniformValue("useIntersectionShader",false);
        this->shaderProgram->bind();
        obj->draw(GL_LINE);
        glLineWidth(2.0f);
    }
    if ( !this->shaderProgram->log().isEmpty() )
    {
        qDebug() << this->shaderProgram->log() ;
    }

    this->shaderProgram->release();

    //glutWireCube(1.0);
    glPopMatrix();

    if (drawText)
    {
        GLText text;
        text.init(this->width(),this->height(),glWhite);
        text.enterTextInputMode();
        text.draw("nTurns= " + util::stringify<GLfloat>(helicoidRotationAngle/objectScale) );
        text.draw("helicoidRotationAngleDeg= " + util::stringify<GLfloat>(helicoidRotationAngle/objectScale*360.0) );
        text.draw("frameNumber= " + util::stringify<GLfloat>(frameNumber) );
        text.draw("eyeZ= " + util::stringify<GLfloat>(eyeZ) );
        text.draw("objectScale= " + util::stringify<GLfloat>(objectScale) );
        text.draw("T=( " + util::stringify<GLfloat>(objectTranslation.x())+","+ util::stringify<GLfloat>(objectTranslation.y())+","+ util::stringify<GLfloat>(objectTranslation.z()) +")");
        text.leaveTextInputMode();
    }
    getGLerrors();
}

/**
 * @brief ShaderWidget::paintGL
 */
void ShaderWidget::paintGL()
{
    qglClearColor(Qt::black);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ////////////////////////////////////////
    draw();
    saveFrame();
    helicoidRotationAngle+=objectScale/viewsNumber;
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
    float FOV=60.0;
    gluPerspective(FOV, (float)width / (float)height, 0.001, 10000.0);
    glMatrixMode(GL_MODELVIEW);

}

void ShaderWidget::wheelEvent(QWheelEvent *e)
{
    eyeZ+=1E-3*e->delta()*objectScale*0.1;
}

void ShaderWidget::keyPressEvent(QKeyEvent *e)
{
    switch ( e->key() )
    {
    case Qt::Key_Q:
    {
        delete obj;
        QApplication::exit(0);
    }
    case Qt::Key_Plus:
    {
        objectScale+=1.0;
        break;
    }
    case Qt::Key_Minus:
    {
        objectScale-=1.0;
        break;
    }
    case Qt::Key_8:
    {
        objectTranslation.y() += 0.01*objectScale;
        break;
    }
    case Qt::Key_2:
    {
       objectTranslation.y() -= 0.01*objectScale;
        break;
    }
    case Qt::Key_4:
    {
        objectTranslation.x() -= 0.01*objectScale;
        break;
    }
    case Qt::Key_6:
    {
        objectTranslation.x() += 0.01*objectScale;
        break;
    }
    case Qt::Key_W:
    {
        viewCount++;
        helicoidRotationAngle+=objectScale/viewsNumber;
        break;
    }
    case Qt::Key_S:
    {
        viewCount--;
        helicoidRotationAngle-=objectScale/viewsNumber;
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
