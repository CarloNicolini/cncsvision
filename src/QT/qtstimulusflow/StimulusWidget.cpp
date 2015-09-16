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

#include <QtGui>
#include <QtOpenGL>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include "Mathcommon.h"
#include "StimulusWidget.h"
#include "Grid.h"
#include "MainWindow.h"
#include "QCustomPlot.h"


using namespace Eigen;
using namespace mathcommon;

/**
 * @brief StimulusWidget::StimulusWidget
 * @param parent
 */
StimulusWidget::StimulusWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    timer = new QTimer(this);

    connect(timer,SIGNAL(timeout()),this,SLOT(repaint()));


    frameNumber=0;
    isSavingFrames=false;
    planeDef=0.4;
    planeTilt=0;
    planeSlant=45;
    stimSize=80;
    drawGrid=false;
    drawMask=false;
    maskRadius = 50;
    stimPoints=100;
    correctShear=true;
    computeOpticFlow=false;
    constantDefMode=true;
    backprojectionActive=false;
    stimTranslating=false;
    oscillationAmplitude=125;
    oscillationPeriod=80;

    grid.init(Vector3d(-stimSize/2,stimSize/2,0),Vector3d(stimSize/2,stimSize/2,0),Vector3d(-stimSize/2,-stimSize/2,0),Vector3d(stimSize/2,-stimSize/2,0));

    stimMotion = SINUSOIDAL;
    planeDir = DOWNUP;

    if (computeOpticFlow)
    {
        customPlot = new QCustomPlot();
        customPlot->resize(400,400);
        customPlot->setTitle("Optic flow realtime plots");

        customPlot->addGraph(); // magenta line
        customPlot->graph(0)->setPen(QPen(Qt::magenta));

        customPlot->addGraph(); // red line
        customPlot->graph(1)->setPen(QPen(Qt::red));

        customPlot->addGraph(); // blue line
        customPlot->graph(2)->setPen(QPen(Qt::blue));

        customPlot->addGraph(); // green line
        customPlot->graph(3)->setPen(QPen(Qt::green));

        customPlot->addGraph(); // black line
        customPlot->graph(4)->setPen(QPen(Qt::black));

        customPlot->addGraph(); // gray line
        customPlot->graph(5)->setPen(QPen(Qt::gray));

        QCPLegend legend(customPlot);
        legend.setVisible(true);

        customPlot->setRangeZoom(Qt::Vertical);
        customPlot->setRangeDrag(Qt::Horizontal | Qt::Vertical);
        customPlot->legend->setVisible(true);
        customPlot->xAxis->setLabel("Frames");
        customPlot->yAxis->setLabel("Optic flows a1,...,a6");
        for (int i=0; i<6; i++)
            customPlot->graph(i)->setName("a" + QString::number(i));
        customPlot->xAxis->setAutoTickStep(true);
        customPlot->setupFullAxesBox();

        // make left and bottom axes transfer their ranges to right and top axes:
        connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
        connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));
        // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
        customPlot->show();
    }
    timer->start(16);
    velocities.resize(6,1);
}

/**
 * @brief StimulusWidget::repaint
 */
void StimulusWidget::repaint()
{
    timeFrame+=1;

    if (computeOpticFlow)
    {
        int frameDelay = 100;
        double mmax =std::numeric_limits<double>::max(), mmin = std::numeric_limits<double>::min();
        for (int i=0; i<6; i++)
        {
            mmax = std::max(mmax,velocities(i));
            mmin = std::min(mmin,velocities(i));

            customPlot->graph(i)->addData( timeFrame,  velocities(i) );
            // remove data of lines that's outside visible range:
            customPlot->graph(i)->removeDataBefore(timeFrame-frameDelay);
        }
        customPlot->yAxis->setRange(1.1*mmin,1.1*mmax);
        customPlot->xAxis->setRange(timeFrame,frameDelay, Qt::AlignRight);

    }

    switch (stimMotion)
    {
    case SAWTOOTH:

        periodicValue = oscillationAmplitude*mathcommon::sawtooth(timeFrame,oscillationPeriod);
        break;
    case TRIANGLE:
        periodicValue = oscillationAmplitude*mathcommon::trianglewave(timeFrame,oscillationPeriod);
        break;
    case SINUSOIDAL:
        periodicValue = oscillationAmplitude*sin(3.14*timeFrame/(oscillationPeriod));
        break;
    }

    stimTransformation.translation() << 0,0,focalDistance;
    if (stimTranslating)
    {
        switch (planeDir)
        {
        case UPDOWN:
            stimTransformation.translation().y() = periodicValue;
            break;
        case DOWNUP:
            stimTransformation.translation().y() = -periodicValue;
            break;
        case LEFTRIGHT:
            stimTransformation.translation().x() = -periodicValue;
            break;
        case RIGHTLEFT:
            stimTransformation.translation().x() = periodicValue;
            break;
        }
    }
    else
    {
        stimTransformation.translation() << 0,0,focalDistance;
    }

    paintGL();
    if (computeOpticFlow)
        customPlot->replot();
}

/**
 * @brief StimulusWidget::initStimulus
 */
void StimulusWidget::initStimulus()
{
    focalDistance=-418.5;
    redDotsPlane.setNpoints(stimPoints);  //XXX controllare densita di distribuzione dei punti
    redDotsPlane.setDimensions(stimSize,stimSize,0.1);
    redDotsPlane.compute();
    stimDrawer.setSpheres(false);
    stimDrawer.setStimulus(&redDotsPlane);
    stimDrawer.initList(&redDotsPlane,glBlack,4);

    //screen.setWidthHeight(width(), 768.0/1024.0*width());
    screen.setWidthHeight(width(),height());
    screen.setOffset(0,0);
    timeFrame=0;
    screen.setFocalDistance(focalDistance);
    cam.init(screen);
}

/**
 * @brief StimulusWidget::setSlant
 * @param angle
 */
void StimulusWidget::setSlant(double angle)
{
    planeSlant = angle;
    emit slantChanged(angle);
}

/**
 * @brief StimulusWidget::setTilt
 * @param angle
 */
void StimulusWidget::setTilt(double angle)
{
    planeTilt = angle;
    emit tiltChanged(angle);
}

/**
 * @brief StimulusWidget::setDef
 * @param val
 */
void StimulusWidget::setDef(double val)
{
    planeDef=val;
    emit defChanged(planeDef);
}

/**
 * @brief StimulusWidget::setFocalDistance
 * @param val
 */
void StimulusWidget::setFocalDistance(double val)
{
    focalDistance=val;
    emit focalDistanceChanged(val);
}

/**
 * @brief StimulusWidget::setBackProjectionActive
 * @param val
 */
void StimulusWidget::setBackProjectionActive(bool val)
{
    backprojectionActive=val;
    emit backprojectionChanged(backprojectionActive);
}

/**
 * @brief StimulusWidget::setPlaneDirection
 * @param p
 */
void StimulusWidget::setPlaneDirection(int p)
{
    switch (p)
    {
    case UPDOWN:
        planeDir = UPDOWN;
        break;
    case DOWNUP:
        planeDir = DOWNUP;
        break;
    case LEFTRIGHT:
        planeDir = LEFTRIGHT;
        break;
    case RIGHTLEFT:
        planeDir = RIGHTLEFT;
        break;
    default:
        cerr << "error" << endl;
        break;
    }
    emit planeDirChanged(p);
}

/**
 * @brief StimulusWidget::setPeriod
 * @param val
 */
void StimulusWidget::setPeriod(double val)
{
    oscillationPeriod=val;
    emit periodChanged(val);
}

/**
 * @brief StimulusWidget::setAmplitude
 * @param val
 */
void StimulusWidget::setAmplitude(double val)
{
    oscillationAmplitude=val;
    emit amplitudeChanged(val);
}

/**
 * @brief StimulusWidget::setConstantDefMode
 * @param val
 */
void StimulusWidget::setConstantDefMode(bool val)
{
    constantDefMode=val;
    emit constantDefModeChanged(val);
}

/**
 * @brief StimulusWidget::setDrawGrid
 * @param val
 */
void StimulusWidget::setDrawGrid(bool val)
{
    drawGrid=val;
    emit drawGridChanged(drawGrid);
}

/**
 * @brief StimulusWidget::setStimulusSize
 * @param val
 */
void StimulusWidget::setStimulusSize(double val)
{
    stimSize = val;
    grid.init(Vector3d(-stimSize/2,stimSize/2,0),Vector3d(stimSize/2,stimSize/2,0),Vector3d(-stimSize/2,-stimSize/2,0),Vector3d(stimSize/2,-stimSize/2,0));
    redDotsPlane.setDimensions(stimSize,stimSize,0.1);
    redDotsPlane.compute();
    stimDrawer.setSpheres(false);
    stimDrawer.setStimulus(&redDotsPlane);
    stimDrawer.initList(&redDotsPlane,glBlack,4);
    emit stimulusSizeChanged(stimSize);
}

/**
 * @brief StimulusWidget::setStimulusMotion
 * @param val
 */
void StimulusWidget::setStimulusMotion(int val)
{
    switch (val)
    {
    case SAWTOOTH:
        stimMotion = SAWTOOTH;
        break;
    case TRIANGLE:
        stimMotion = TRIANGLE;
        break;
    case SINUSOIDAL:
        stimMotion = SINUSOIDAL;
        break;
    default :
        stimMotion=SAWTOOTH;
        break;
    }
    emit stimulusMotionChanged(val);
}

/**
 * @brief StimulusWidget::setDrawMask
 * @param val
 */
void StimulusWidget::setDrawMask(bool val)
{
    drawMask = val;
    if (drawMask)
        glEnable(GL_STENCIL_TEST);
    else
        glDisable(GL_STENCIL_TEST);

    emit drawMaskChanged(drawMask);
}

/**
 * @brief StimulusWidget::setMaskRadius
 * @param val
 */
void StimulusWidget::setMaskRadius(double val)
{
    maskRadius=val;
    emit maskRadiusChanged(maskRadius);
}

/**
 * @brief StimulusWidget::setStimPoints
 * @param val
 */
void StimulusWidget::setStimPoints(int val)
{
    stimPoints=val;
    redDotsPlane.setNpoints(stimPoints);
    redDotsPlane.compute();
    stimDrawer.setStimulus(&redDotsPlane);
    stimDrawer.initList(&redDotsPlane,glBlack,4);
}

/**
 * @brief StimulusWidget::setCorrectShear
 * @param val
 */
void StimulusWidget::setCorrectShear(bool val)
{
    correctShear=val;
    emit shearCorrected(correctShear);
}

/**
 * @brief StimulusWidget::setTranslation
 * @param val
 */
void StimulusWidget::setTranslation(bool val)
{
    stimTranslating=val;
    emit translationChanged(stimTranslating);
}

/**
 * @brief StimulusWidget::minimumSizeHint
 * @return
 */
QSize StimulusWidget::minimumSizeHint() const
{
    return QSize(150, 150);
}

/**
 * @brief StimulusWidget::sizeHint
 * @return
 */
QSize StimulusWidget::sizeHint() const
{
    return QSize(400, 400);
}

/**
 * @brief StimulusWidget::initializeGL
 */
void StimulusWidget::initializeGL()
{
    int argc = 1;
    char *argv[] = {""};
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL ); // Set up a basic display buffer (only single buffered for now)

    initStimulus();

    qglClearColor(Qt::white);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
    glClearStencil(0);

    // Set depth buffer clear value
    glClearDepth(1.0);
    // Enable depth test */
    glEnable(GL_DEPTH_TEST);
    // Set depth function
    glDepthFunc(GL_LEQUAL);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    stimTransformation = Eigen::Affine3d::Identity();
}

/**
 * @brief StimulusWidget::paintGL
 */
void StimulusWidget::paintGL()
{
    Eigen::Affine3d modelTransformation=Eigen::Affine3d::Identity();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
    glClearStencil(0);
    qglClearColor(Qt::white);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // Now draw the stimulus
    if (correctShear)
        cam.setEye(Vector3d(stimTransformation.translation().x(),stimTransformation.translation().y(),0));
    else
        cam.setEye(Vector3d::Zero());
    double theta=0.0;
    double angle=0.0;

    //glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushMatrix();
    glLoadIdentity();
    glMultMatrixd(stimTransformation.data());

    double planeOnset=1;

    if (!constantDefMode)
    {
        switch ( (int)planeTilt )
        {
        case 0:
            glRotated(-planeSlant,0,1,0);
            if (backprojectionActive)
                glScaled(1/sin(toRadians( -90-planeSlant)),1,1);	//backprojection phase
            break;
        case 90:
            glRotated(-planeSlant,1,0,0);
            if (backprojectionActive)
                glScaled(1,1/sin(toRadians( -90-planeSlant )),1); //backprojection phase
            break;
        case 180:
            glRotated(planeSlant,0,1,0);
            if (backprojectionActive)
                glScaled(1/sin(toRadians( -90-planeSlant )),1,1); //backprojection phase
            break;
        case 270:
            glRotated(planeSlant,1,0,0);
            if (backprojectionActive)
                glScaled(1,1/sin(toRadians( -90-planeSlant )),1); //backprojection phase
            break;
        }
    }
    else
    {
        switch ( (int)planeTilt )
        {
        case 0:
            theta = -acos(exp(-0.346574+0.5*planeDef-planeDef/2*(1-planeOnset*periodicValue/oscillationAmplitude)));
            glRotated(toDegrees(theta),0,1,0);
            if (backprojectionActive)
                glScaled(1/sin(toRadians( -90-planeSlant)),1,1);	//backprojection phase
            break;
        case 90:
            theta = -acos(exp(-0.346574+0.5*planeDef-planeDef/2*(1-planeOnset*periodicValue/oscillationAmplitude)));
            glRotated( toDegrees(theta),1,0,0);
            if (backprojectionActive)
                glScaled(1,1/sin(toRadians( -90-planeSlant )),1); //backprojection phase
            break;
        case 180:
            theta = acos(exp(-0.346574+0.5*planeDef-planeDef/2*(1+planeOnset*periodicValue/oscillationAmplitude)));
            glRotated(toDegrees(theta),0,1,0);
            if (backprojectionActive)
                glScaled(1/sin(toRadians( -90-planeSlant )),1,1); //backprojection phase
            break;
        case 270:
            theta = acos(exp(-0.346574+0.5*planeDef-planeDef/2*(1+planeOnset*periodicValue/oscillationAmplitude)));
            glRotated( toDegrees(theta),1,0,0);
            if (backprojectionActive)
                glScaled(1,1/sin(toRadians( -90-planeSlant )),1); //backprojection phase
            break;
        }
    }

    stimDrawer.draw();
    glGetDoublev(GL_MODELVIEW_MATRIX,modelTransformation.matrix().data());
    modelTransformation.matrix() = modelTransformation.matrix().eval();

    if (drawGrid)
        grid.draw();

    glPopMatrix();



    if ( drawMask )
    {
        glPushMatrix();
        glLoadIdentity();
        Vector3d projCoords = project(modelTransformation.translation());

        circleMask( projCoords.x(), projCoords.y(), maskRadius );
        glPopMatrix();
    }

    if ( computeOpticFlow )
    {
        Eigen::Affine3d activeViewer=Eigen::Affine3d::Identity();
        activeViewer.translation() = stimTransformation.translation() - Vector3d(0,0,-focalDistance);
        vector<Eigen::Vector3d> ppoints = stimDrawer.projectStimulusPoints(modelTransformation,activeViewer,cam,focalDistance,screen,Vector3d::Zero(),false,false);

        // Now we must compute the angles alpha1, beta1, alpha2, beta2, alpha3, beta3 which are the angles
        // which the extremal points form with respect to the subject eye
        alpha[1]=alpha[0];
        beta[1]=beta[0];
        // alpha is the "pitch" angle, beta is the "yaw" angle
        for ( int i=0; i<3; i++)
        {
            alpha[0](i) =     ( atan2( ppoints.at(i).x(), abs(focalDistance) ) );
            beta[0](i)     =  ( atan2( ppoints.at(i).y(), abs(focalDistance)  ) );
        }

        MatrixXd angVelocities;
        angVelocities.resize(6,1);
        angVelocities.setZero(6,1);

        for (int i=0; i<3; i++)
        {
            angVelocities(i,0) = alpha[0](i)-alpha[1](i);
            angVelocities(i+3,0) =  beta[0](i)-beta[1](i);
        }

        angVelocities /= 0.0167;

        MatrixXd coeffMatrix(6,6);
        coeffMatrix <<
                       1, alpha[0](0),   beta[0](0), 0, 0, 0,
                0, 0,    0,    1,alpha[0](0),beta[0](0),
                1, alpha[0](1),   beta[0](1), 0, 0, 0,
                0, 0,    0,    1,alpha[0](1),beta[0](1),
                1, alpha[0](2),   beta[0](2), 0, 0, 0,
                0, 0,    0,    1,alpha[0](2),beta[0](2)
                ;
        //fullPivHouseholderQR
        MatrixXd v = (coeffMatrix.colPivHouseholderQr().solve(angVelocities)).eval();
        if ( abs(v.array().sum())>1E-9 )
            velocities=v;
    }

    if ( isSavingFrames )
        saveFrame("");
    else
    {
        this->frameNumber=0;
    }
}

/**
 * @brief StimulusWidget::resizeGL
 * @param width
 * @param height
 */
void StimulusWidget::resizeGL(int width, int height)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glViewport(0,0, this->width(), this->height());
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

/**
 * @brief StimulusWidget::~StimulusWidget
 */
StimulusWidget::~StimulusWidget()
{
    if (computeOpticFlow)
    {
        customPlot->close();
        delete customPlot;
    }
}

/**
 * @brief StimulusWidget::saveFrame
 * @param outputDirectory
 */
void StimulusWidget::saveFrame(const QString &outputDirectory)
{
    QString frameString;
    frameString.sprintf("%04d", frameNumber);
    frameNumber++;
    QString outputFrameFileName;
    if (this->saveFramesDirectory.isEmpty())
        outputFrameFileName = QDir::currentPath() + QDir::separator()+ QString(QDir::separator())+ QString("img_") + frameString +".png";
    else
        outputFrameFileName = saveFramesDirectory + QDir::separator()+ QString(QDir::separator())+ QString("img_") + frameString +".png";

    QImage frame = this->grabFrameBuffer();
    int dpm = ceil(72.0 / 0.0254); // ~300 DPI
    frame.setDotsPerMeterX(dpm);
    frame.setDotsPerMeterY(dpm);
    if (  !frame.save( outputFrameFileName, NULL, -1) )
    {
        QMessageBox::warning(this,"Warning saving image","Can't save to file "+outputFrameFileName);
    }
}

/**
 * @brief StimulusWidget::setSaveFramesDirectory
 * @param dir
 */
void StimulusWidget::setSaveFramesDirectory(const QString &dir)
{
    this->saveFramesDirectory=dir;
}
