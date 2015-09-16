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
#include "MainWindow.h"
#include "ui_MainWindow.h"

/**
 * @brief MainWindow::MainWindow
 * @param parent
 */
MainWindow::MainWindow(QWidget *parent ) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->ui->volumetricGLWidget->setOutputFolderName(QDir::currentPath());
    this->ui->lineEditOutputDirectory->setText(QDir::currentPath());

    // Connections
    QObject::connect(this->ui->doubleSpinBoxOffsetX,SIGNAL(valueChanged(double)),this,SLOT(onDoubleSpinboxOffsetXChanged(double)));
    QObject::connect(this->ui->doubleSpinBoxOffsetY,SIGNAL(valueChanged(double)),this,SLOT(onDoubleSpinboxOffsetYChanged(double)));
    QObject::connect(this->ui->doubleSpinBoxOffsetZ,SIGNAL(valueChanged(double)),this,SLOT(onDoubleSpinboxOffsetZChanged(double)));
    QObject::connect(this->ui->doubleSpinBoxRotationAngle,SIGNAL(valueChanged(double)),this,SLOT(onDoubleSpinboxRotationAngleChanged(double)));
    QObject::connect(this->ui->doubleSpinBoxObjectSize,SIGNAL(valueChanged(double)),this,SLOT(onDoubleSpinboxObjectSizeChanged(double)));
    QObject::connect(this->ui->spinBoxNSlices,SIGNAL(valueChanged(int)),this,SLOT(onSpinboxNSlicesChanged(int)));
    QObject::connect(this->ui->pushButtonRandomizeSpheres,SIGNAL(clicked()),this,SLOT(onPushbuttonRandomizeSpheresPressed()));
    QObject::connect(this->ui->pushButtonStartFramesGeneration,SIGNAL(clicked()),this,SLOT(onPushButtonStartFramesGenerationClicked()));
    QObject::connect(this->ui->pushButtonSetOutputDirectory,SIGNAL(clicked()),this,SLOT(onPushbuttonsetoutputdirectoryClicked()));

}

/**
 * @brief MainWindow::~MainWindow
 */
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

/**
 * @brief MainWindow::keyPressEvent
 * @param e
 */
void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape || e->key() == Qt::Key_Q )
        close();
    else
        QWidget::keyPressEvent(e);
}

/**
 * @brief MainWindow::onPushbuttonsetoutputdirectoryClicked
 */
void MainWindow::onPushbuttonsetoutputdirectoryClicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open output directory"),
                                                    QDir::currentPath(),
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    this->ui->volumetricGLWidget->setOutputFolderName(dir);
    this->ui->lineEditOutputDirectory->setText(dir);
}

/**
 * @brief MainWindow::onPushButtonStartFramesGenerationClicked
 */
void MainWindow::onPushButtonStartFramesGenerationClicked()
{
    this->ui->volumetricGLWidget->setHelicoidZeroColor(0);
    this->ui->volumetricGLWidget->setCurrentGLColor(Qt::black);
    this->ui->volumetricGLWidget->update();
    this->ui->volumetricGLWidget->startFramesGeneration();
    this->ui->volumetricGLWidget->update();
    QMessageBox::information(this,"Files saved successfully","Files have been saved to\n "+this->ui->lineEditOutputDirectory->text());
    this->ui->volumetricGLWidget->setCurrentGLColor(Qt::gray);
    this->ui->volumetricGLWidget->setHelicoidZeroColor(128);
    this->update();
}

/**
 * @brief MainWindow::onDoubleSpinboxOffsetXChanged
 * @param value
 */
void MainWindow::onDoubleSpinboxOffsetXChanged(double value)
{
    this->ui->volumetricGLWidget->setOffset(value,ui->doubleSpinBoxOffsetY->value(),ui->doubleSpinBoxOffsetZ->value());
    this->ui->volumetricGLWidget->update();
}

/**
 * @brief MainWindow::onDoubleSpinboxOffsetYChanged
 * @param value
 */
void MainWindow::onDoubleSpinboxOffsetYChanged(double value)
{
    this->ui->volumetricGLWidget->setOffset(ui->doubleSpinBoxOffsetX->value(),value,ui->doubleSpinBoxOffsetZ->value());
    this->ui->volumetricGLWidget->update();
}

/**
 * @brief MainWindow::onDoubleSpinboxOffsetZChanged
 * @param value
 */
void MainWindow::onDoubleSpinboxOffsetZChanged(double value)
{
    this->ui->volumetricGLWidget->setOffset(ui->doubleSpinBoxOffsetX->value(),ui->doubleSpinBoxOffsetY->value(),value);
    this->ui->volumetricGLWidget->update();
}

/**
 * @brief MainWindow::onDoubleSpinboxRotationChanged
 * @param value
 */
void MainWindow::onDoubleSpinboxRotationAngleChanged(double value)
{
    double delta = value*360.0/this->ui->spinBoxNSlices->value();
    this->ui->volumetricGLWidget->setRotation(delta/180.0*M_PI);
    this->ui->volumetricGLWidget->update();
}

/**
 * @brief MainWindow::onDoubleSpinboxObjectSizeChanged
 * @param value
 */
void MainWindow::onDoubleSpinboxObjectSizeChanged(double value)
{
    this->ui->volumetricGLWidget->setObjectScale(value);
}

/**
 * @brief MainWindow::onSpinboxNSlicesChanged
 */
void MainWindow::onSpinboxNSlicesChanged(int value)
{
    this->ui->volumetricGLWidget->setSlicesNumber(value);
}

/**
 * @brief MainWindow::onPushbuttonRandomizeSpheresPressed
 */
void MainWindow::onPushbuttonRandomizeSpheresPressed()
{
    this->ui->volumetricGLWidget->randomizeSpheres(ui->spinBoxNSpheres->value(),ui->spinBoxSpheresRadiusMin->value(),ui->spinBoxSpheresRadiusMax->value());
}

/**
 * @brief MainWindow::on_spinBoxSpheresRadiusMin_valueChanged
 * @param arg1
 */
void MainWindow::on_spinBoxSpheresRadiusMin_valueChanged(int arg1)
{
    if (arg1 >= this->ui->spinBoxSpheresRadiusMax->value() )
    {
        this->ui->spinBoxSpheresRadiusMax->setValue(arg1+1);
    }

}

/**
 * @brief MainWindow::on_spinBoxSpheresRadiusMax_valueChanged
 * @param arg1
 */
void MainWindow::on_spinBoxSpheresRadiusMax_valueChanged(int arg1)
{
    if (arg1 < this->ui->spinBoxSpheresRadiusMin->value() )
    {
        this->ui->spinBoxSpheresRadiusMin->setValue(arg1-1);
    }
}

/**
 * @brief MainWindow::on_checkBoxCameraViewMode_clicked
 * @param checked
 */
void MainWindow::on_checkBoxCameraViewMode_clicked(bool checked)
{
    this->ui->volumetricGLWidget->toggleStandardGL(checked);
}

/**
 * @brief MainWindow::on_pushButtonLoad2Dpoints_clicked
 */
void MainWindow::on_pushButtonLoad2Dpoints_clicked()
{
    QString points2Dfilename = QFileDialog::getOpenFileName(this,"Select 2D projected points correspondence",QDir::currentPath(),"*.txt");
    this->ui->lineEdit2DCalibrationPoints->setText(points2Dfilename);
    this->ui->volumetricGLWidget->setPoints2Dfilename(points2Dfilename);
    if ( points2Dfilename.isEmpty() )
    {
        QMessageBox::critical(this,"Error","Select a valid (non-empty)  file for 2D points. Now exiting...");
        QCoreApplication::exit(-1);
    }
}

/**
 * @brief MainWindow::on_pushButtonLoad3Dpoints_clicked
 */
void MainWindow::on_pushButtonLoad3Dpoints_clicked()
{
    QString points3Dfilename = QFileDialog::getOpenFileName(this,"Select 3D projected points correspondence",QDir::currentPath(),"*.txt");
    this->ui->lineEdit3DCalibrationPoints->setText(points3Dfilename);
    this->ui->volumetricGLWidget->setPoints3Dfilename(points3Dfilename);
    if ( points3Dfilename.isEmpty() )
    {
        QMessageBox::critical(this,"Error","Select a valid (non-empty)  file for 3D points. Now exiting...");
        QCoreApplication::exit(-1);
    }
}

/**
 * @brief MainWindow::on_checkBoxUseCalibratedView_clicked
 * @param checked
 */
void MainWindow::on_checkBoxUseCalibratedView_clicked(bool checked)
{
    this->ui->volumetricGLWidget->toggleUseCalibratedGLView();
}
