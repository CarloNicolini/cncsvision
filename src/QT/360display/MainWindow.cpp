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


#include <QObject>
#include <QtGui>
#include <QSlider>
#include "GLWidgetGeometry.h"
#include "MainWindow.h"
#include "ExperimentalParameters.h"
#include "ui_MainWindow.h"

//! [0]
MainWindow::MainWindow(QWidget *parent ) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Simulation 360 light field display");

    // GLWidget has been new'd so experimental parameters is an instance of it.

    ui->doubleSpinBoxProjCenterX->setRange(-200.0,200.0);
    ui->doubleSpinBoxProjCenterY->setRange(00.0,2000.0);
    ui->doubleSpinBoxProjCenterZ->setRange(-200.0,200.0);

    ui->doubleSpinBoxProjFocal->setRange(0.0,1000.0);
    ui->doubleSpinBoxProjAperture->setRange(-45,45);
    ui->doubleSpinBoxProjAperture->setValue(ui->glWidgetGeometry->getParameters()->projectorApertureDegrees);

    ui->doubleSpinBoxProjAreaEdge->setRange(0,500);
    ui->doubleSpinBoxProjAreaEdge->setValue(ui->glWidgetGeometry->getParameters()->projectorAreaEdge);

    ui->doubleSpinBoxProjRoll->setRange(0,360);
    ui->doubleSpinBoxProjPitch->setRange(0,180);
    ui->doubleSpinBoxProjRoll->setSingleStep(1);
    ui->doubleSpinBoxProjPitch->setSingleStep(1);

    QObject::connect(ui->doubleSpinBoxProjPitch,SIGNAL(valueChanged(double)),ui->glWidgetGeometry,SLOT(setProjectorPitch(double)));
    QObject::connect(ui->doubleSpinBoxProjRoll,SIGNAL(valueChanged(double)),ui->glWidgetGeometry,SLOT(setProjectorRoll(double)));
    QObject::connect(ui->doubleSpinBoxProjAperture,SIGNAL(valueChanged(double)),ui->glWidgetGeometry,SLOT(setProjectorAperture(double)));
    QObject::connect(ui->doubleSpinBoxProjAreaEdge,SIGNAL(valueChanged(double)),ui->glWidgetGeometry,SLOT(setProjectedAreaEdge(double)));

    ui->doubleSpinBoxProjPitch->setValue(ui->glWidgetGeometry->getParameters()->projectorPitch);
    ui->doubleSpinBoxProjRoll->setValue(ui->glWidgetGeometry->getParameters()->projectorRoll);

    ui->doubleSpinBoxViewerDistance->setRange(0,3000);
    ui->doubleSpinBoxViewerHeight->setRange(-200,200);

    ui->doubleSpinBoxMirrorAngle->setRange(0,360);
    ui->doubleSpinBoxMirrorAngle->setValue(ui->glWidgetGeometry->getParameters()->mirrorRotationDegree);
    ui->glWidgetGeometry->setMirrorRotation(ui->doubleSpinBoxMirrorAngle->value());

    // Set default values
    ui->doubleSpinBoxProjCenterX->setValue(ui->glWidgetGeometry->getParameters()->projCenter.x());
    ui->doubleSpinBoxProjCenterY->setValue(ui->glWidgetGeometry->getParameters()->projCenter.y());
    ui->doubleSpinBoxProjCenterZ->setValue(ui->glWidgetGeometry->getParameters()->projCenter.z());
    ui->doubleSpinBoxProjFocal->setValue(ui->glWidgetGeometry->getParameters()->projectorFocal);

    QObject::connect(ui->doubleSpinBoxProjCenterX,SIGNAL(valueChanged(double)),ui->glWidgetGeometry,SLOT(setProjectorPositionX(double)));
    QObject::connect(ui->doubleSpinBoxProjCenterY,SIGNAL(valueChanged(double)),ui->glWidgetGeometry,SLOT(setProjectorPositionY(double)));
    QObject::connect(ui->doubleSpinBoxProjCenterZ,SIGNAL(valueChanged(double)),ui->glWidgetGeometry,SLOT(setProjectorPositionZ(double)));
    QObject::connect(ui->doubleSpinBoxProjFocal,SIGNAL(valueChanged(double)),ui->glWidgetGeometry,SLOT(setProjectorFocal(double)));

    // Connect viewer angle spinboxes
    ui->doubleSpinBoxViewerDistance->setSingleStep(10);
    ui->doubleSpinBoxViewerDistance->setValue(ui->glWidgetGeometry->getParameters()->viewerDistance);

    ui->doubleSpinBoxViewerHeight->setValue(ui->glWidgetGeometry->getParameters()->viewerHeight);

    ui->doubleSpinBoxMirrorFreq->setValue(ui->glWidgetGeometry->getParameters()->mirrorAngularSpeedDegForSec);

    QObject::connect(ui->doubleSpinBoxMirrorFreq,SIGNAL(valueChanged(double)),ui->glWidgetGeometry,SLOT(setMirrorRotationSpeed(double)));

    QObject::connect(ui->doubleSpinBoxViewerDistance,SIGNAL(valueChanged(double)),ui->glWidgetGeometry,SLOT(setViewerDistance(double)));
    QObject::connect(ui->doubleSpinBoxViewerHeight,SIGNAL(valueChanged(double)),ui->glWidgetGeometry,SLOT(setViewerHeight(double)));

    QObject::connect(ui->doubleSpinBoxMirrorAngle,SIGNAL(valueChanged(double)),ui->glWidgetGeometry,SLOT(setMirrorRotation(double)) );
    QObject::connect(ui->checkboxAutorotation,SIGNAL(clicked(bool)),ui->glWidgetGeometry,SLOT(setAutorotation(bool)));
    QObject::connect(ui->checkBoxViewRaysBackward,SIGNAL(clicked(bool)),ui->glWidgetGeometry,SLOT(setViewRaysBackward(bool)));
    QObject::connect(ui->checkboxViewRaysForward,SIGNAL(clicked(bool)),ui->glWidgetGeometry,SLOT(setViewRaysForward(bool)));
    QObject::connect(ui->spinBoxRayIndex,SIGNAL(valueChanged(int)),ui->glWidgetGeometry,SLOT(setRayVertexIndex(int)));

    QObject::connect(ui->actionOpen_mesh_file_obj,SIGNAL(triggered()),this,SLOT(onActionOpenMeshFileObjTriggered()));

    // Connect angle variation
    QObject::connect(ui->glWidgetGeometry,SIGNAL(angleChanged(double)),ui->doubleSpinBoxMirrorAngle,SLOT(setValue(double)));
    this->onActionOpenMeshFileObjTriggered();

    QObject::connect(ui->glWidgetGeometry,SIGNAL(modelViewProjectionMatrixChanged(const Eigen::Matrix4d&)),ui->glShaderWidget,SLOT(setModelviewProjectionMatrix(const Eigen::Matrix4d &)));
    QObject::connect(ui->glWidgetGeometry,SIGNAL(modelViewMatrixChanged(const Eigen::Matrix4d &)),this,SLOT(fillModelViewTextEdit(const Eigen::Matrix4d &)));
    QObject::connect(ui->glWidgetGeometry,SIGNAL(projectionMatrixChanged(const Eigen::Matrix4d &)),this,SLOT(fillModelViewProjMatrixTextEdit(const Eigen::Matrix4d &)));

    // Finally connect geometry widget  and shader widget
    QObject::connect(ui->glWidgetGeometry,SIGNAL(modelChanged()),ui->glShaderWidget,SLOT(repaint()));
    // Temporarily disable the unused spinboxes
    ui->doubleSpinBoxProjAperture->setEnabled(false);
    ui->doubleSpinBoxProjPitch->setEnabled(false);
    ui->doubleSpinBoxProjRoll->setEnabled(false);

    qApp->installEventFilter(this);
}

MainWindow::~MainWindow()
{

}

/**
 * @brief Window::eventFilter
 * @return
 */
bool MainWindow::eventFilter(QObject *, QEvent *)
{
    return false;
}

//! [1]

//! [2]
void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape || e->key() == Qt::Key_Q )
        close();
    else
        QWidget::keyPressEvent(e);
}

void MainWindow::onActionOpenMeshFileObjTriggered()
{
    ///QString filename = QFileDialog::getOpenFileName(this,"Open file...","","Wavefront object mesh (*.obj);;");
    QString filename("/home/carlo/workspace/cncsvisioncmake/data/objmodels/face.obj");
    if (filename.isEmpty())
    {
        QMessageBox mbox(this);
        mbox.setIcon(QMessageBox::Critical);
        mbox.setText("You must select a valid file.Now exiting!");
        mbox.setStandardButtons(QMessageBox::Close);
        mbox.show();
        mbox.exec();
        exit(0);
    }
    QFileInfo info(filename);
    if (!info.exists())
    {
        QMessageBox mbox(this);
        mbox.setIcon(QMessageBox::Critical);
        mbox.setText("File does not exist!");
        //  mbox.setWindowModality(Qt::WindowModal);
        mbox.setStandardButtons(QMessageBox::Close);
        mbox.show();
        mbox.exec();
    }
    else
    {
        ui->glWidgetGeometry->loadMesh(filename);
        ui->glWidgetGeometry->computeProjectionsBackward();
        ui->glWidgetGeometry->repaint();
    }
    ui->spinBoxRayIndex->setRange(0,ui->glWidgetGeometry->getParameters()->meshVertices-1);

    QObject::connect(ui->glWidgetGeometry,SIGNAL(verticesChanged(GLfloat*,unsigned int)),ui->glShaderWidget,SLOT(setVertices(GLfloat*,unsigned int)));
    ui->glShaderWidget->setParameters( ui->glWidgetGeometry->getParameters());
}

/**
 * @brief MainWindow::fillModelViewTextEdit
 * @param projectiveMatrix
 */
void MainWindow::fillModelViewProjMatrixTextEdit(const Eigen::Matrix4d &modelviewProjection)
{
    QString row;
    for (int i=0; i<4;i++)
    {
        for (int j=0; j<4; j++)
        {
            row += QString::number(modelviewProjection(i,j),'g',3) + "    ";
        }
        row+="\n";
    }
    ui->textEditModelviewProj->setText(row);
}

/**
 * @brief MainWindow::fillProjectorPlaneCoords
 * @param s
 */
void MainWindow::fillModelViewTextEdit(const Eigen::Matrix4d &modelviewProjection)
{
    QString row;
    for (int i=0; i<4;i++)
    {
        for (int j=0; j<4; j++)
        {
            row += QString::number(modelviewProjection(i,j),'g',3) + "    ";
        }
        row+="\n";
    }
    ui->textEditModelViewMatrix->setText(row);
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::quit();
}
