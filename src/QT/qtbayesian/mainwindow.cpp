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

#include "mainwindow.h"
#include "ui_mainwindow.h"

// Standard includes
#include <iostream>
#include <fstream>

// QT includes
#include <QDate>
#include <QTime>
#include <QFileDialog>
#include <QTimer>
#include <QFuture>
#include <QThread>
#include <QProgressDialog>
#include <QtConcurrentRun>
#include "matrixpaintwidget.h"
#include "ModelThread.h"

// Eigen includes
#include <Eigen/Core>

// Boost include
#include <boost/thread.hpp>
#include <boost/bind.hpp>

// CNCSVision includes
#include "ParametersLoader.h"
#include "Util.h"
#include "BayesNetwork.h"

using namespace util;
using std::fstream;
using std::ifstream;

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    progress.setMinimum(0);
    progress.setMaximum(100);
    ui->statusbar->addWidget(&progress);
    matrixWidget = new MatrixPaintWidget();
    matrixWidget->setMainWindow(this);
    matrixWidget->show();
    ui->runButton->setCheckable(true);
    ui->stopButton->setCheckable(true);
    modelThread=new ModelThread(this);
}

MainWindow::~MainWindow()
{
    delete modelThread;
    delete matrixWidget;
    delete ui;
}

void MainWindow::on_actionOpen_file_triggered()
{
    QFileDialog dialog;
    QString filename = dialog.getOpenFileName(this,"Parameters file to open","../libs/statistic/","*.bay");

    ifstream parametersFile;
    parametersFile.open(filename.toStdString().c_str());
    ParametersLoader par;
    par.loadParameterFile(parametersFile);

    // Set gibbs sampler parameters
    ui->spinBoxBurnInIterations->setValue(str2num<int>(par.find("nGibbsBurnIn")));
    ui->spinBoxValidIterations->setValue(str2num<int>(par.find("nGibbsValid")));
    ui->doubleSpinBoxDomMin->setValue(str2num<double>(par.find("domainMin")));
    ui->doubleSpinBoxDomMax->setValue(str2num<double>(par.find("domainMax")));
    ui->doubleSpinBoxDomStep->setValue(str2num<double>(par.find("domainStep")));

    // Set plane orientation priors
    ui->doubleSpinBoxSigmaGx->setValue(str2num<double>(par.find("sigmaGx")));
    ui->doubleSpinBoxSigmaGy->setValue(str2num<double>(par.find("sigmaGy")));
    ui->doubleSpinBoxGMin->setValue(str2num<double>(par.find("gMin")));
    ui->doubleSpinBoxGMax->setValue(str2num<double>(par.find("gMax")));
    ui->doubleSpinBoxGStep->setValue(str2num<double>(par.find("gStep")));

    // Epsilon of flows priors
    ui->doubleSpinBoxE1->setValue(str2num<double>(par.find("epsilon1")));
    ui->doubleSpinBoxE2->setValue(str2num<double>(par.find("epsilon2")));
    ui->doubleSpinBoxE3->setValue(str2num<double>(par.find("epsilon3")));
    ui->doubleSpinBoxE4->setValue(str2num<double>(par.find("epsilon4")));
    ui->doubleSpinBoxE5->setValue(str2num<double>(par.find("epsilon5")));
    ui->doubleSpinBoxE6->setValue(str2num<double>(par.find("epsilon6")));

    // Mean of flows priors
    ui->doubleSpinBoxMa1->setValue(str2num<double>(par.find("ma1")));
    ui->doubleSpinBoxMa2->setValue(str2num<double>(par.find("ma2")));
    ui->doubleSpinBoxMa3->setValue(str2num<double>(par.find("ma3")));
    ui->doubleSpinBoxMa4->setValue(str2num<double>(par.find("ma4")));
    ui->doubleSpinBoxMa5->setValue(str2num<double>(par.find("ma5")));
    ui->doubleSpinBoxMa6->setValue(str2num<double>(par.find("ma6")));

    // Translation priors
    ui->doubleSpinBoxMomega0X->setValue(str2num<double>(par.find("mOmega0x")));
    ui->doubleSpinBoxMomega0Y->setValue(str2num<double>(par.find("mOmega0y")));
    ui->doubleSpinBoxMomega0Z->setValue(str2num<double>(par.find("mOmega0z")));

    ui->doubleSpinBoxSigmaOmega0X->setValue(str2num<double>(par.find("sigmaOmega0x")));
    ui->doubleSpinBoxSigmaOmega0Y->setValue(str2num<double>(par.find("sigmaOmega0y")));
    ui->doubleSpinBoxSigmaOmega0Z->setValue(str2num<double>(par.find("sigmaOmega0z")));

    // Rotation priors
    ui->doubleSpinBoxMOmegaX->setValue(str2num<double>(par.find("mOmegax")));
    ui->doubleSpinBoxMOmegaY->setValue(str2num<double>(par.find("mOmegay")));
    ui->doubleSpinBoxMOmegaZ->setValue(str2num<double>(par.find("mOmegaz")));

    ui->doubleSpinBoxSigmaOmegaX->setValue(str2num<double>(par.find("sigmaOmegax")));
    ui->doubleSpinBoxSigmaOmegaY->setValue(str2num<double>(par.find("sigmaOmegay")));
    ui->doubleSpinBoxSigmaOmegaZ->setValue(str2num<double>(par.find("sigmaOmegaz")));

    parametersFile.close();

    updateParameters();
}

void MainWindow::on_actionSave_parameters_file_triggered()
{
    QFileDialog dialog;
    dialog.setAcceptMode(QFileDialog::AcceptSave);//To save files,default is open mode.
    dialog.setNameFilter(".bay");
    QString filename = dialog.getSaveFileName(this,"Save parameters file...",QDir::currentPath(),".bay");
    filename.replace(".bay","");
    filename.append(".bay");

    ofstream outputparams;
    outputparams.open(filename.toStdString().c_str());
    // Set gibbs sampler parameters
    outputparams << "# File created: " << QDate::currentDate().toString().toStdString() <<  " " << QTime::currentTime().toString().toStdString() << endl;
    outputparams << "nDimensions: 6" << endl;
    outputparams << "nGibbsBurnIn: " << ui->spinBoxBurnInIterations->value() << endl;
    outputparams << "nGibbsValid: " << ui->spinBoxValidIterations->value() << endl;
    outputparams << "domainMin: " << ui->doubleSpinBoxDomMin->value() << endl;
    outputparams << "domainMax: " << ui->doubleSpinBoxDomMax->value() << endl;
    outputparams << "domainStep: " << ui->doubleSpinBoxDomStep->value() << endl;

    outputparams << "sigmaGx: " << ui->doubleSpinBoxSigmaGx->value() << endl;
    outputparams << "sigmaGy: " << ui->doubleSpinBoxSigmaGy->value() << endl;
    outputparams << "gMin: " << ui->doubleSpinBoxGMin->value() << endl;
    outputparams << "gMax: "  << ui->doubleSpinBoxGMax->value() << endl;
    outputparams << "gStep: " <<  ui->doubleSpinBoxGStep->value() << endl;

    outputparams << "epsilon1: " << ui->doubleSpinBoxE1->value() << endl;
    outputparams << "epsilon2: " << ui->doubleSpinBoxE2->value() << endl;
    outputparams << "epsilon3: " <<  ui->doubleSpinBoxE3->value() << endl;
    outputparams  <<"epsilon4: "<< ui->doubleSpinBoxE4->value() << endl;
    outputparams << "epsilon5: " << ui->doubleSpinBoxE5->value() << endl;
    outputparams << "epsilon6: " << ui->doubleSpinBoxE6->value() << endl;
    outputparams << "ma1: " << ui->doubleSpinBoxMa1->value() << endl;
    outputparams << "ma2: " << ui->doubleSpinBoxMa2->value() << endl;
    outputparams << "ma3: " << ui->doubleSpinBoxMa3->value() << endl;
    outputparams << "ma4: " << ui->doubleSpinBoxMa4->value() << endl;
    outputparams << "ma5: " << ui->doubleSpinBoxMa5->value() << endl;
    outputparams << "ma6: " <<ui->doubleSpinBoxMa6->value() << endl;

    outputparams << "mOmega0x: " << ui->doubleSpinBoxMomega0X->value() << endl;
    outputparams << "mOmega0y: " << ui->doubleSpinBoxMomega0Y->value() << endl;
    outputparams << "mOmega0z: " << ui->doubleSpinBoxMomega0Z->value() << endl;
    outputparams << "mOmegax: " << ui->doubleSpinBoxMOmegaX->value() << endl;
    outputparams << "mOmegay: " << ui->doubleSpinBoxMOmegaY->value() << endl;
    outputparams << "mOmegaz: " << ui->doubleSpinBoxMOmegaZ->value() << endl;

    outputparams << "sigmaOmega0x: " << ui->doubleSpinBoxSigmaOmega0X->value() << endl;
    outputparams << "sigmaOmega0y: " << ui->doubleSpinBoxSigmaOmega0Y->value() << endl;
    outputparams << "sigmaOmega0z: " << ui->doubleSpinBoxSigmaOmega0Z->value() << endl;
    outputparams << "sigmaOmegax: " << ui->doubleSpinBoxSigmaOmegaX->value() << endl;
    outputparams << "sigmaOmegay: " << ui->doubleSpinBoxSigmaOmegaY->value() << endl;
    outputparams << "sigmaOmegaz: " << ui->doubleSpinBoxSigmaOmegaZ->value() << endl;

    outputparams.close();
}

void MainWindow::on_actionQuit_triggered()
{
    matrixWidget->close();
    this->close();
}

void MainWindow::compute()
{
    if ( !modelThread->isRunning() )
    {
        modelThread->start();

        // Connette percentuale con barra di avanzamento
        connect(modelThread,SIGNAL(rowPercent(int)),&progress,SLOT(setValue(int)));
        // Connette immagine emessa con disegnatore immagine
        connect(modelThread, SIGNAL(theImage(const QImage &)),
                matrixWidget, SLOT(updatePixmap(const QImage &)));
    }
}

void MainWindow::updateParameters()
{
    nDimensions=    6;
    nGibbsBurnIn=   ui->spinBoxBurnInIterations->value();
    nGibbsValid =  ui->spinBoxValidIterations->value();
    sigmaOmega0x=   ui->doubleSpinBoxSigmaOmega0X->value();
    sigmaOmega0y=   ui->doubleSpinBoxSigmaOmega0Y->value();
    sigmaOmega0z=   ui->doubleSpinBoxSigmaOmega0Z->value();

    sigmaOmegax=  ui->doubleSpinBoxSigmaOmegaX->value();
    sigmaOmegay=   ui->doubleSpinBoxSigmaOmegaY->value();
    sigmaOmegaz=   ui->doubleSpinBoxSigmaOmegaZ->value();

    mOmega0x=ui->doubleSpinBoxMomega0X->value();
    mOmega0y=ui->doubleSpinBoxMomega0Y->value();
    mOmega0z=ui->doubleSpinBoxMomega0Z->value();
    mOmegax=ui->doubleSpinBoxMOmegaX ->value();
    mOmegay=ui->doubleSpinBoxMOmegaY->value();
    mOmegaz=ui->doubleSpinBoxMOmegaZ->value();
    sigmaGx=        ui->doubleSpinBoxSigmaGx->value();
    sigmaGy=         ui->doubleSpinBoxSigmaGy->value();
    ma1=            ui->doubleSpinBoxMa1->value();
    ma2=             ui->doubleSpinBoxMa2->value();
    ma3=            ui->doubleSpinBoxMa3->value();
    ma4=             ui->doubleSpinBoxMa4->value();
    ma5=             ui->doubleSpinBoxMa5->value();
    ma6=             ui->doubleSpinBoxMa6->value();
    epsilon1=       ui->doubleSpinBoxE1->value();
    epsilon2=        ui->doubleSpinBoxE2->value();
    epsilon3=       ui->doubleSpinBoxE3->value();
    epsilon4=       ui->doubleSpinBoxE4->value();
    epsilon5=       ui->doubleSpinBoxE5->value();
    epsilon6=       ui->doubleSpinBoxE6->value();
    domainMin=     ui->doubleSpinBoxDomMin->value();
    domainMax=    ui->doubleSpinBoxDomMax->value();
    domainStep=    ui->doubleSpinBoxDomStep->value();

    gxMin=gyMin=    ui->doubleSpinBoxGMin->value();
    gxMax=gyMax=   ui->doubleSpinBoxGMax->value();
    gStep=         ui->doubleSpinBoxGStep->value();

    domain.clear();
    domain.resize(nDimensions);
    for (unsigned int i=0; i<nDimensions; i++)
        domain.at(i) = pair<double,double>(domainMin,domainMax);

    dgranularity= (abs(domainMax-domainMin))/domainStep;
    dgranularityGx=abs(gxMax-gxMin)/gStep;
    dgranularityGy=abs(gyMax-gyMin)/gStep;
    granularityGx = floor(dgranularityGx+0.5);
    granularityGy = floor(dgranularityGy+0.5);
}

void MainWindow::checkAllParameters()
{
    cerr << "===== MODEL PARAMETERS =====" << endl << endl;
    cerr << "nDimensions= " << nDimensions << endl;
    cerr << "nGibbsBurnIn= " << nGibbsBurnIn << endl;
    cerr << "nGibbsValid= " << nGibbsValid << endl << endl;
    cerr << "Σ ω0x= " << sigmaOmega0x << endl;
    cerr << "Σ ω0y= " << sigmaOmega0y << endl;
    cerr << "Σ ω0z= " << sigmaOmega0z << endl << endl;
    cerr << "Σ ωx= " << sigmaOmegax << endl;
    cerr << "Σ ωy= " << sigmaOmegay << endl;
    cerr << "Σ ωz= " << sigmaOmegaz << endl << endl;
    cerr << "Σ gx= " << sigmaGx << endl;
    cerr << "Σ gy= " << sigmaGy << endl << endl;
    cerr << "ma1= " << ma1 << endl;
    cerr << "ma2= " << ma2 << endl;
    cerr << "ma3= " << ma3 << endl;
    cerr << "ma4= " << ma4 << endl;
    cerr << "ma5= " << ma5 << endl;
    cerr << "ma6= " << ma6 << endl << endl;
    cerr << "ε1= " << epsilon1 << endl ;
    cerr << "ε2= " << epsilon2 << endl;
    cerr << "ε3= " << epsilon3 << endl;
    cerr << "ε4= " << epsilon4 << endl;
    cerr << "ε5= " << epsilon5 << endl;
    cerr << "ε6= " << epsilon6 << endl;
    cerr << "domainMin= " << domainMin << endl;
    cerr << "domainMax= " << domainMax << endl;
    cerr << "domainStep= " << domainStep << endl << endl;
    cerr << "gMin= " << gxMin << endl;
    cerr << "gMax= " << gxMax << endl;
    cerr << "gStep= " << gStep << endl;
    cerr << "mOmega0x= " << mOmega0x << endl;
    cerr << "mOmega0y= " << mOmega0y << endl;
    cerr << "mOmega0z= " << mOmega0z << endl;
    cerr << "mOmegax= " << mOmegax << endl;
    cerr << "mOmegay= " << mOmegay << endl;
    cerr << "mOmegaz= " << mOmegaz << endl;
}

void MainWindow::on_runButton_clicked()
{

    modelThread->requestStop(false);
    progress.setValue(0);
    ui->runButton->setChecked(true);
    updateParameters();
    compute();
    ui->runButton->setChecked(false);
     matrixWidget->show();
}

void MainWindow::on_stopButton_clicked()
{
    progress.setValue(0);
    modelThread->requestStop(true);
    ui->stopButton->setChecked(true);

    ui->stopButton->setChecked(false);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    matrixWidget->close();
    this->close();
}
