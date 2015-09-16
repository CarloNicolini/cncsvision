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

#ifndef STIMULUSWIDGET_H
#define STIMULUSWIDGET_H

#include <QWidget>
#include <QLayout>
#include <QGLWidget>
#include <QtGui>
#include <QtOpenGL>
#include <Eigen/Core>
#include "Stimulus.h"
#include "BoxNoiseStimulus.h"
#include "StimulusDrawer.h"
#include "QCustomPlot.h"
#include "Grid.h"
//#include "MainWindow.h"


class MainWindow;

class StimulusWidget :  public QGLWidget
{
    Q_OBJECT
public:
    StimulusWidget(QWidget *parent = 0);
    ~StimulusWidget();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void initStimulus();
    void setMainWindow(MainWindow *_mainWindow)
    {
        this->mainWin    = _mainWindow;
    };

    void setSavingFrames(bool val)
    {
        this->isSavingFrames=val;
    }

    void closeCustomPlot()
    {
        this->customPlot->close();
    }

public slots:
    void setSlant(double);
    void setTilt(double);
    void setDef(double);
    void setPlaneDirection(int );
    void setPeriod(double);
    void setAmplitude(double);
    void setFocalDistance(double);
    void setBackProjectionActive(bool);
    void setConstantDefMode(bool);
    void setDrawGrid(bool);
    void setStimulusSize(double);
    void setStimulusMotion(int);
    void setDrawMask(bool);
    void setMaskRadius(double);
    void setStimPoints(int);
    void setCorrectShear(bool);
    void setTranslation(bool);
    void setSaveFramesDirectory(const QString &dir);
    void paintGL();

    void repaint();

 signals:
            void xRotationChanged(int);
            void slantChanged(double);
            void tiltChanged(double);
            void defChanged(double);
            void periodChanged(int);
            void amplitudeChanged(int);
            void planeDirChanged(int);
            void backprojectionChanged(bool);
            void constantDefModeChanged(bool);
            void focalDistanceChanged(double);
            void drawGridChanged(bool);
            void stimulusSizeChanged(double);
            void stimulusMotionChanged(int);
            void drawMaskChanged(bool);
            void maskRadiusChanged(double);
            void stimPointsChanged(int);
            void translationChanged(bool);
            void shearCorrected(bool);


protected:
    void initializeGL();


    void resizeGL(int width, int height);
    void saveFrame(const QString &outputDirectory);

private:

    double planeSlant,planeTilt,planeDef,stimSize,maskRadius;
    double timeFrame, timerDeltaT, oscillationPeriod, oscillationAmplitude, periodicValue;
    int frameNumber;
    bool isSavingFrames;
    QTimer *timer;
    Eigen::Affine3d stimTransformation;
    double focalDistance;
    bool backprojectionActive, constantDefMode,drawGrid, drawMask,correctShear, computeOpticFlow,stimTranslating;
    int stimPoints;
    BoxNoiseStimulus redDotsPlane;
    StimulusDrawer stimDrawer;
    VRCamera cam;
    Screen screen;
    QPoint lastPos;
    QString saveFramesDirectory;

    QCustomPlot *customPlot;
    enum PlaneDirection
    {
        UPDOWN,
        DOWNUP,
        LEFTRIGHT,
        RIGHTLEFT
    } planeDir;

    enum StimulusMotion
    {
        SAWTOOTH,
        TRIANGLE,
        SINUSOIDAL
    } stimMotion;

    // Optic flow computation things
    Vector3d alpha[2],beta[2];
    QSplitter *splitter;
    MainWindow *mainWin;

    MatrixXd velocities;
    Grid grid;
};

#endif // STIMULUSWIDGET_H
