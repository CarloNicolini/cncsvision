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

#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    timer = new QTimer(this);
    timer->start(16);

    ui->stimWidget->setMainWindow(this);
    // XXX QUI mettere i connect con stimuluswidget
    //   connect(xSlider, SIGNAL(valueChanged(int)), stimWidget, SLOT(setXRotation(int)));
    //   connect(stimWidget, SIGNAL(xRotationChanged(int)), xSlider, SLOT(setValue(int)));
    // Connect per timer di animazione


    connect(timer,SIGNAL(timeout()), ui->stimWidget,SLOT(updateGL()));
    // connect( ui->spinBoxTimerMS, SIGNAL(valueChanged(int)), ui->stimWidget, SLOT(setTimerUpdate(int)));
    connect( ui->doubleSpinBoxSlant, SIGNAL(valueChanged(double)), ui->stimWidget, SLOT(setSlant(double)));
    connect(ui->doubleSpinBoxTilt, SIGNAL(valueChanged(double)),ui->stimWidget,SLOT(setTilt(double)));
    connect(ui->doubleSpinBoxPeriod, SIGNAL(valueChanged(double)),ui->stimWidget,SLOT(setPeriod(double)));
    connect(ui->doubleSpinBoxAmplitude, SIGNAL(valueChanged(double)),ui->stimWidget,SLOT(setAmplitude(double)));
    connect(ui->comboBoxDirections, SIGNAL(currentIndexChanged(int)),ui->stimWidget,SLOT(setPlaneDirection(int)));
    connect(ui->doubleSpinBoxDef, SIGNAL(valueChanged(double)),ui->stimWidget,SLOT(setDef(double)));
    connect(ui->doubleSpinBoxFocalDistance, SIGNAL(valueChanged(double)) , ui->stimWidget, SLOT(setFocalDistance(double)));
    connect(ui->checkBoxBackProjection, SIGNAL(clicked(bool)) , ui->stimWidget, SLOT(setBackProjectionActive(bool)));
    connect(ui->checkBoxConstantDef, SIGNAL(clicked(bool)) , ui->stimWidget, SLOT(setConstantDefMode(bool)));
    connect(ui->checkBoxDrawGrid,SIGNAL(clicked(bool)),ui->stimWidget,SLOT(setDrawGrid(bool)));
    connect(ui->doubleSpinBoxStimSize,SIGNAL(valueChanged(double)),ui->stimWidget,SLOT(setStimulusSize(double)));
    connect(ui->comboBoxStimMotion, SIGNAL(currentIndexChanged(int)),ui->stimWidget,SLOT(setStimulusMotion(int)));


    connect(ui->doubleSpinBoxMaskRadius, SIGNAL(valueChanged(double)),ui->stimWidget,SLOT(setMaskRadius(double)));
    connect(ui->checkBoxCircularMask, SIGNAL(clicked(bool)), ui->doubleSpinBoxMaskRadius,SLOT(setEnabled(bool)));
    connect(ui->checkBoxCircularMask, SIGNAL(clicked(bool)), ui->stimWidget,SLOT(setDrawMask(bool)));

    connect(ui->spinBoxNPoints, SIGNAL(valueChanged(int)), ui->stimWidget,SLOT(setStimPoints(int)));
    connect(ui->checkBoxCorrectShear,SIGNAL(clicked(bool)),ui->stimWidget,SLOT(setCorrectShear(bool)));
    connect(ui->checkBoxTranslation,SIGNAL(clicked(bool)),ui->stimWidget,SLOT(setTranslation(bool)));

    setWindowTitle(tr("Optic flow simulator"));

}

MainWindow::~MainWindow()
{
    ui->stimWidget->closeCustomPlot();
}

void MainWindow::on_actionQuit_triggered()
{

    ui->stimWidget->closeCustomPlot();
    this->close();
    QApplication::exit(0);
}

void MainWindow::on_actionInfo_triggered()
{
    QMessageBox::information(this,"Information","Optic flow simulator for a passive viewer.\nFor informations carlo.nicolini@iit.it, carlo.fantoni@iit.it");
}

void MainWindow::on_checkBox_2_clicked(bool checked)
{
    ui->stimWidget->setSavingFrames(checked);
}

void MainWindow::on_pushButtonSaveToDir_clicked(bool checked)
{
    ui->stimWidget->setSaveFramesDirectory(QFileDialog::getExistingDirectory(this, tr("Open Directory"), QDir::currentPath() ));
}
