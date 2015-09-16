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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <Eigen/Core>
#include <QMainWindow>
#include <QImage>
#include <QProgressBar>
#include <fstream>

class ParametersLoader;
class MatrixPaintWidget;
class ModelThread;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // Widgets
    Ui::MainWindow *ui;
    MatrixPaintWidget *matrixWidget;
    ModelThread *modelThread;
    QProgressBar progress;

    // Public methods to control main program
    void update();
    void compute();
    void updateParameters();
    void checkAllParameters();

    // Internal model variables
    unsigned int nDimensions, nGibbsBurnIn, nGibbsValid;
    int granularityGx,granularityGy;
    double domainMin, domainMax, domainStep;
    // Diagonal covariance matrices element
    double sigmaOmega0x, sigmaOmega0y, sigmaOmega0z;
    double sigmaOmegax, sigmaOmegay, sigmaOmegaz;
    double sigmaGx, sigmaGy;
    double epsilon1,epsilon2,epsilon3,epsilon4,epsilon5,epsilon6;
    double mOmega0x,mOmega0y,mOmega0z,mOmegax,mOmegay,mOmegaz;
    // Mean value of gaussians
    double ma1, ma2,ma3, ma4,ma5, ma6;
    double gxMin, gxMax, gyMin, gyMax, gStep;
    double dgranularity,dgranularityGx, dgranularityGy;
    std::vector < std::pair<double, double> > domain;
    Eigen::MatrixXd Pgxgy;

public slots:
    void on_stopButton_clicked();
    void on_runButton_clicked();
    void on_actionQuit_triggered();
    void on_actionSave_parameters_file_triggered();
    void on_actionOpen_file_triggered();
    void closeEvent(QCloseEvent *);
};

#endif // MAINWINDOW_H
