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
#include "InfoDialog.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void sendCommand(const std::string &port, const std::string &command);
    void update();
    void resetAllBoxes();
    InfoDialog *dialog;
private:
    void on_pushButtonReadObj_clicked();
    void on_pushButtonReadMirr_clicked();
    void on_pushButtonReadMon_clicked();
private slots:
    void on_boxObjAbsmmZ_valueChanged(double );
    void on_boxObjAbsmmY_valueChanged(double );
    void on_boxObjAbsmmX_valueChanged(double );
    void on_boxMirrAbsmmZ_valueChanged(double );
    void on_boxMonAbsmmZ_valueChanged(double );
    void on_boxMonAbsmmX_valueChanged(double );
    void on_boxObjRelmmZ_valueChanged(double );
    void on_boxObjRelmmY_valueChanged(double );
    void on_boxObjRelmmX_valueChanged(double );
    void on_boxMirrRelmmZ_valueChanged(double );
    void on_boxMonRelmmZ_valueChanged(double );
    void on_boxMonRelmmX_valueChanged(double val );
    void on_pushButtonHomeObjZ_clicked();
    void on_pushButtonHomeObjY_clicked();
    void on_pushButtonHomeObjX_clicked();
    void on_pushButtonMoveObjRel_clicked();
    void on_pushButtonMoveObj_clicked();
    void on_pushButtonHomeMirrZ_clicked();
    void on_pushButtonMoveMirrRel_clicked();
    void on_pushButtonMoveMirr_clicked();
    void on_pushButtonMoveMonRel_clicked();
    void on_pushButtonMoveMonAbs_clicked();
    void on_pushButtonHomeMonZ_clicked();
    void on_homeObject_clicked();
    void on_homeMirror_clicked();
    void on_homeMonitor_clicked();
    void on_pushButtonHomeMonX_clicked();
    void on_actionQuit_triggered();
    void on_actionHelp_triggered();


};

#endif // MAINWINDOW_H
