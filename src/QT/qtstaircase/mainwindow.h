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

#include <QMainWindow>
#include <QtGui>
#include <fstream>

#include "ParStaircase.h"
#include "QCustomPlot.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

// Action slots
private slots:
    void on_actionNew_experiment_triggered();
    void on_actionOpenFile_triggered();
    void on_actionExportResultsToFile_triggered();
    void on_actionQuit_triggered();
// Connections slots
private slots:
    void on_buttonStart_clicked();
    void on_spinBoxLastInvAverage_valueChanged(int );
    void on_spinBoxNStaircases_valueChanged(int );


// Private variables
    void on_comboBoxDrawStyle_currentIndexChanged(int index);

    void on_checkBox_stateChanged(int );

    void on_actionExport_graph_to_pdf_triggered();

    void on_actionSaveFile_triggered();

private:
    // Widgets
    Ui::MainWindow *ui;
    int lastRunNStaircase;
    ParStaircase parStaircase;

    QStringList list;
    QVector<bool> simulatedResponses;
    QVector<int> simulatedIDS,simulatedNInversions,simulatedStaircaseSteps,simulatedNTrial;
    QVector<double> simulatedStates;

    // QCustomPlot related things
    QVector<QColor> colors;
    //std::map<QString,int> ids;
    std::map<int,QString> id2string;
    std::map<int, std::map<int,double> > id2inversions2states;

    QCPGraph::ScatterStyle plotStyle;
    bool drawPsychometric;
// Private methods
private:
    void setupGraphThings();
    void updateStats();
    void updateGraph(int n,QCPGraph::ScatterStyle=QCPGraph::ssNone);
    void setupPsychometric();
    void initParStaircase();
    void updateTable();
};

#endif // MAINWINDOW_H
