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

#include <iostream>
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "SerialStream.h"
#include "Util.h"
#include <QThread>
#include <Eigen/Core>
#include <stdexcept>
#include <QMessageBox>
#include "InfoDialog.h"

static const double HOMEFOCALDISTANCE=-418.5;

#define MOTORSTEPSIZE 0.005

using namespace util;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonHomeMonX_clicked()
{
    string buffer = string("F,C,")+string("S1M")+stringify<int>((ui->boxMonSpeedX->value())>2000 ? 2000 : ui->boxMonSpeedX->value())+
            string(",I1M0,IA1M-0,R");

    sendCommand("COM3",buffer);
}

void MainWindow::on_pushButtonHomeMonZ_clicked()
{
    string buffer = string("F,C,")+string("S2M")+stringify<int>((ui->boxMonSpeedZ->value())>2000 ? 2000 : (ui->boxMonSpeedZ->value()))+
            string(",I2M0,IA2M-0,R");

    sendCommand("COM3",buffer);
}

void MainWindow::on_homeMonitor_clicked()
{
    // Note that motor 1 is z axis, motor 2 is x axis
    string buffer = string("F,C,")+string("S1M")+stringify<int>((ui->boxMonSpeedZ->value())>2000 ? 2000 : (ui->boxMonSpeedZ->value()))+string(",S2M")+stringify<int>((ui->boxMonSpeedX->value())>2000 ? 2000 : ui->boxMonSpeedX->value())+
            string(",I1M0,I2M0,IA1M-0,IA2M-0,R");

    sendCommand("COM3",buffer);
}

void MainWindow::on_homeMirror_clicked()
{
    string buffer = string("F,C,")+string("S1M")+stringify<int>(ui->boxMirrSpeedZ->value())+
            string(",I1M0,IA1M-0,R");

    sendCommand("COM5",buffer);
}


void MainWindow::on_pushButtonMoveMonAbs_clicked()
{
    int nStepsX = ui->boxMonAbsIndexX->value();
    int nStepsZ = ui->boxMonAbsIndexZ->value();
    // Don't send nothing in this case
    if (nStepsX==nStepsZ && nStepsX==0)
        return;

    string buffer =
            string("F,C,");

    if (nStepsX!=0)
    {
        buffer +=
                string("S1M")+stringify<int>(ui->boxMonSpeedX->value())+
                string(",IA1M")+stringify<int>(nStepsX)+string(",");
    }
    if (nStepsZ!=0)
    {
        buffer +=
                string("S2M")+stringify<int>(ui->boxMonSpeedZ->value())+
                string(",IA2M")+stringify<int>(nStepsZ);
    }
    buffer+=string(",R");
    // Remove the
    QString qbuffer(buffer.c_str());
    qbuffer.replace(",,",",");
    buffer=qbuffer.toStdString();

    sendCommand("COM3",buffer);
}

void MainWindow::on_pushButtonMoveMonRel_clicked()
{
    int nStepsX = ui->boxMonRelIndexX ->value();
    int nStepsZ = ui->boxMonRelIndexZ ->value();
    if (nStepsX==nStepsZ && nStepsX==0)
        return;
    string buffer("F,C,");
    buffer+= string("S1M")+stringify<int>(ui->boxMonSpeedX->value())+","+string("S2M")+stringify<int>(ui->boxMonSpeedZ->value());
    if (nStepsX!=0)
    {
        buffer +=
                string(",I1M")+stringify<int>(nStepsX)+string(",");
    }
    if (nStepsZ!=0)
    {
        buffer +=
                string(",I2M")+stringify<int>(nStepsZ);
    }

    buffer+=string(",R");
    // Remove the
    QString qbuffer(buffer.c_str());
    qbuffer.replace(",,",",");
    buffer=qbuffer.toStdString();

    sendCommand("COM3",buffer);
}

void MainWindow::on_pushButtonReadMon_clicked()
{/*
    try
    {
    SerialStream serial("COM5");
    }
    catch (...)
    {
        std::string what = "Cannot open port COM5";
        QMessageBox::warning( this,  "Error",QString(what.c_str())+" or this computer has no such port!");
        return;
    }
    SerialStream serial("COM5");    // bleah it's very unelegant!!
    char Z[10],Y[10],X[10];
    try
    {
    // Home object on Z axis
    serial << 'Z' << endl;
    serial >> Z;

    serial << 'X' << endl;
    serial >> X;

    serial << 'Y' << endl;
    serial >> Y;
    }
    catch (...)
    {
        std::string what = "Can't read motor indices on port COM5";
        QMessageBox::warning( this,  "Error",QString(what.c_str())+" or this computer has no such port!");
    }
    double x = str2num<int>(string(&Z))*MOTORSTEPSIZE;
    double y = str2num<int>(string(&Y))*MOTORSTEPSIZE;
    double z = str2num<int>(string(&X))*MOTORSTEPSIZE;

    Eigen::Vector3i positionIntegers( str2num<int>(Z),str2num<int>(X),str2num<int>(Y) );
    Eigen::Vector3d position(x,y,z);
    std::string strpos = stringify<Eigen::Matrix<double,3,1> >(position.transpose());
    ui->plainTextEdit->appendPlainText(strpos.c_str());
    */
}

void MainWindow::on_pushButtonMoveMirr_clicked()
{
    // XXX controllare!
    int nStepsZ = ui->boxMirrAbsIndexZ->value();
    // Don't send nothing in this case
    if (nStepsZ ==0)
        return;

    string buffer =
            string("F,C,");

    if (nStepsZ!=0)
    {
        buffer +=
                string("S1M")+stringify<int>(ui->boxMirrSpeedZ->value())+
                string(",IA1M")+stringify<int>(nStepsZ);
    }
    buffer+=string(",R");
    // Remove the
    QString qbuffer(buffer.c_str());
    qbuffer.replace(",,",",");
    buffer=qbuffer.toStdString();

    sendCommand("COM5",buffer);
}

void MainWindow::on_pushButtonMoveMirrRel_clicked()
{
    // XXX controllare!
    int nStepsZ = ui->boxMirrRelIndexZ->value();
    // Don't send nothing in this case
    if ( nStepsZ ==0)
        return;

    string buffer =
            string("F,C,");

    buffer+=string("S1M")+stringify<int>(ui->boxMirrSpeedZ->value());
    if (nStepsZ!=0)
    {
        buffer +=
                string(",I1M")+stringify<int>(nStepsZ);
    }
    buffer+=string(",R");
    // Remove the
    QString qbuffer(buffer.c_str());
    qbuffer.replace(",,",",");
    buffer=qbuffer.toStdString();

    sendCommand("COM5",buffer);
}

void MainWindow::on_pushButtonHomeMirrZ_clicked()
{
    int speed  = ui->boxMirrSpeedZ->value();
    string buffer = string("F,C,")+string("S1M")+stringify<int>(speed>2000 ? 2000 : speed)+
            string(",I1M0,IA1M-0,R");

    sendCommand("COM5",buffer);
}

void MainWindow::on_pushButtonReadMirr_clicked()
{
    /*
    try
    {
    SerialStream serial("COM3");
    }
    catch (...)
    {
        std::string what = "Cannot open port COM4";
        QMessageBox::warning( this,  "Error",QString(what.c_str())+" or this computer has no such port!");
        return;
    }
    SerialStream serial("COM4");    // bleah it's very unelegant!!
    string Z,X,Y;
    try
    {
    // Home object on Z axis

    serial << "Z" << endl;
    serial >> Z;

    serial << "X" << endl;
    serial >> X;

    serial << "Y" << endl;
    serial >> Y;
    }
    catch (...)
    {
        std::string what = "Can't read motor indices on port COM4";
        QMessageBox::warning( this,  "Error",QString(what.c_str())+" or this computer has no such port!");
    }
    double x = str2num<int>(Z)*MOTORSTEPSIZE;
    double y = str2num<int>(X)*MOTORSTEPSIZE;
    double z = str2num<int>(Y)*MOTORSTEPSIZE;
    Eigen::Vector3i positionIntegers( str2num<int>(Z),str2num<int>(X),str2num<int>(Y) );
    Eigen::Vector3d position(x,y,z);
    std::string strpos = stringify<Eigen::Matrix<double,3,1> >(position.transpose());
    ui->plainTextEdit->appendPlainText(strpos.c_str());
    */
}

void MainWindow::on_pushButtonMoveObj_clicked()
{
    // XXX controllare gli ordini!!!
    int nstepsX = ui->boxObjAbsIndexX->value();
    int nstepsY = ui->boxObjAbsIndexY->value();
    int nstepsZ = ui->boxObjAbsIndexZ->value();

    if ( nstepsX==0 && nstepsY==0 && nstepsZ ==0)
        return;
    string buffer("F,C,");
    buffer+=
            string("S2M")+stringify<int>(ui->boxObjSpeedX->value())+
            string(",S3M")+stringify<int>(ui->boxObjSpeedY->value())+
            string(",S4M")+stringify<int>(ui->boxObjSpeedZ->value());

    if (nstepsX!=0)
        buffer+= string(",IA2M")+stringify<int>(nstepsX);
    if (nstepsY!=0)
        buffer+= string(",IA3M")+stringify<int>(nstepsY);
    if (nstepsZ!=0)
        buffer+= string(",IA4M")+stringify<int>(nstepsZ);

    buffer+=string(",R");

    QString qbuffer(buffer.c_str());
    qbuffer.replace(",,",",");
    buffer=qbuffer.toStdString();

    sendCommand("COM5",buffer);
}

void MainWindow::on_pushButtonMoveObjRel_clicked()
{
    Eigen::Vector3d delta;//(ui->boxObjRelmmX,ui->boxObjRelmmY,ui->boxObjRelmmZ);
    // XXX controllare gli ordini!!!
    // Qui deve prendere il valore esatto degli steps senza la conversione + o - !!!
    int nstepsX =  ui->boxObjRelIndexX->value();
    int nstepsY =  ui->boxObjRelIndexY->value();
    int nstepsZ =  ui->boxObjRelIndexZ->value();
    if ( (nstepsX==nstepsY) && (nstepsZ==nstepsX) && nstepsX==0 )
        return;
    string buffer("F,C,");
    buffer+=
            string("S2M")+stringify<int>(ui->boxObjSpeedX->value())+
            string(",S3M")+stringify<int>(ui->boxObjSpeedY->value())+
            string(",S4M")+stringify<int>(ui->boxObjSpeedZ->value());

    if (nstepsX!=0)
    {
        buffer+= string(",I2M")+stringify<int>(nstepsX);
    }
    if (nstepsY!=0)
        buffer+= string(",I3M")+stringify<int>(nstepsY);
    if (nstepsZ)
        buffer+= string(",I4M")+stringify<int>(nstepsZ);

    buffer+=string(",R");

    QString qbuffer(buffer.c_str());
    qbuffer.replace(",,",",");
    buffer=qbuffer.toStdString();

    sendCommand("COM5",buffer);
}

void MainWindow::sendCommand(const std::string &port, const std::string &command)
{
#ifdef _WIN32
    try
    {
    SerialStream serial(port); //open a standard serial port, baudrate 9600
    ui->plainTextEdit->appendPlainText(QString(port.c_str())+" : " + QString(command.c_str()));
    serial << command << endl;
    char out=0;
    serial >> out;
    }
    catch ( std::exception &ex)
    {
        //std::string what = ex.what();
		std::string what = "Cannot open port "+ port + " " + ex.what();
        QMessageBox::warning( this,  "Error",QString(what.c_str())+" or this computer has no such port!");
    }
#else
    std::cerr << "COM " << port<< ": " << command << endl;
#endif
    ui->plainTextEdit->appendPlainText("Ready");
    ui->plainTextEdit->show();
    this->resetInputContext();
    this->resetAllBoxes();
}

void MainWindow::on_pushButtonReadObj_clicked()
{
    /*
    try
    {
    SerialStream serial("COM3");
    }
    catch (...)
    {
        std::string what = "Cannot open port COM3";
        QMessageBox::warning( this,  "Error",QString(what.c_str())+" or this computer has no such port!");
        return;
    }
    SerialStream serial("COM3");    // bleah it's very unelegant!!
    string Z,X,Y;
    try
    {
    // Home object on Z axis

    serial << "Z" << endl;
    serial >> Z;

    serial << "X" << endl;
    serial >> X;

    serial << "Y" << endl;
    serial >> Y;
    }
    catch (...)
    {
        std::string what = "Can't read motor indices on port COM3";
        QMessageBox::warning( this,  "Error",QString(what.c_str())+" or this computer has no such port!");
    }
    double x = str2num<int>(Z)*MOTORSTEPSIZE;
    double y = str2num<int>(X)*MOTORSTEPSIZE;
    double z = str2num<int>(Y)*MOTORSTEPSIZE;

    Eigen::Vector3i positionIntegers( str2num<int>(Z),str2num<int>(X),str2num<int>(Y) );
    Eigen::Vector3d position(x,y,z);
    std::string strpos = stringify<Eigen::Matrix<double,3,1> >(position.transpose());
    ui->plainTextEdit->appendPlainText(strpos.c_str());
*/
}

void MainWindow::on_pushButtonHomeObjX_clicked()
{
    int speed  = ui->boxObjSpeedX->value();
    string buffer = string("F,C,")+string("S2M")+stringify<int>(speed > 4000 ? 4000 : speed)+
            string(",I2M-0,IA2M-0,R");
    sendCommand("COM5",buffer);
}

void MainWindow::on_pushButtonHomeObjY_clicked()
{
    int speed  = ui->boxObjSpeedY->value();
    string buffer = string("F,C,")+string("S3M")+stringify<int>(speed > 4000 ? 4000 : speed)+
            string(",I3M0,IA3M0,R");

    sendCommand("COM5",buffer);

}

void MainWindow::on_pushButtonHomeObjZ_clicked()
{
    int speed  = ui->boxObjSpeedZ->value();
    string buffer = string("F,C,")+string("S4M")+stringify<int>(speed > 4000 ? 4000 : speed)+
            string(",I4M-0,IA4M-0,R");

    sendCommand("COM5",buffer);

}

void MainWindow::on_homeObject_clicked()
{
    int speedx  = ui->boxObjSpeedX->value();
    int speedy  = ui->boxObjSpeedY->value();
    int speedz  = ui->boxObjSpeedZ->value();

    ui->homeObject->setDown(true);
    // Note that motor 1 is z axis, motor 3 is x axis
    string buffer = string("F,C,")+string("S2M")+stringify<int>(speedz>4000 ? 4000 :speedz )+string(",S3M")+stringify<int>(speedy>4000 ? 4000 :speedy)+string(",S4M")+stringify<int>(speedx>4000 ? 4000 :speedx)+
            string(",(I2M-0,I3M-0,)I4M0,IA2M-0,IA3M-0,IA4M-0,R");

    sendCommand("COM5",buffer);
    ui->homeObject->setDown(false);
}

void MainWindow::on_boxMonRelmmX_valueChanged(double val)
{
        int d = val*200;
    ui->boxMonRelIndexX->setValue(d);
}

void MainWindow::on_boxMonRelmmZ_valueChanged(double val)
{
        int d = val*200;
    ui->boxMonRelIndexZ->setValue(d);
}

void MainWindow::on_boxMirrRelmmZ_valueChanged(double val)
{
        int d = val*200;
    ui->boxMirrRelIndexZ->setValue(d);
}

void MainWindow::on_boxObjRelmmX_valueChanged(double val)
{
        int d = val*200;
    ui->boxObjRelIndexX->setValue(-d);
}

void MainWindow::on_boxObjRelmmY_valueChanged(double val)
{
        int d = val*200;
    ui->boxObjRelIndexY->setValue(-d);
}

void MainWindow::on_boxObjRelmmZ_valueChanged(double val)
{
        int d = val*400;    //XXX qui deve essere metà perchè è stato scelto così
    ui->boxObjRelIndexZ->setValue(d);
}

void MainWindow::on_boxMonAbsmmX_valueChanged(double val)
{
        int d = val*200;
    ui->boxMonAbsIndexX->setValue(d);
}

void MainWindow::on_boxMonAbsmmZ_valueChanged(double val )
{
        int d = val*200;
    ui->boxMonAbsIndexZ->setValue(d);
}

void MainWindow::on_boxMirrAbsmmZ_valueChanged(double val)
{
    int d = val*200;
    ui->boxMirrAbsIndexZ->setValue(d);
}

void MainWindow::on_boxObjAbsmmX_valueChanged(double val)
{
        int d = val*200;
    ui->boxObjAbsIndexX->setValue(-d);
}

void MainWindow::on_boxObjAbsmmY_valueChanged(double val)
{
        int d = val*200;
    ui->boxObjAbsIndexY->setValue(-d);
}

void MainWindow::on_boxObjAbsmmZ_valueChanged(double val)
{
        int d = val*400;    //qui deve essere doppio perchè è un motore diverso con un passo diverso
    ui->boxObjAbsIndexZ->setValue(d);
}

void MainWindow::on_actionQuit_triggered()
{
    close();
}

void MainWindow::on_actionHelp_triggered()
{
        dialog = new InfoDialog(this);
        dialog->show();
}

void MainWindow::resetAllBoxes()
{
    ui->boxMonAbsmmX->setValue(0);
    ui->boxMonAbsmmZ->setValue(0);
    ui->boxMonRelmmX->setValue(0);
    ui->boxMonRelmmZ->setValue(0);
    ui->boxMonRelIndexX->setValue(0);
    ui->boxMonRelIndexZ->setValue(0);
    ui->boxMonAbsIndexX->setValue(0);
    ui->boxMonAbsIndexZ->setValue(0);
    ui->boxMirrAbsmmZ->setValue(0);
    ui->boxMirrRelmmZ->setValue(0);
    ui->boxMirrRelIndexZ->setValue(0);
    ui->boxMirrAbsIndexZ->setValue(0);
    ui->boxObjAbsmmX->setValue(0);
    ui->boxObjRelmmX->setValue(0);
    ui->boxObjRelmmY->setValue(0);
    ui->boxObjRelmmZ->setValue(0);
    ui->boxObjAbsmmY->setValue(0);
    ui->boxObjAbsmmZ->setValue(0);
    ui->boxObjRelIndexX->setValue(0);
    ui->boxObjRelIndexY->setValue(0);
    ui->boxObjRelIndexZ->setValue(0);
    ui->boxObjAbsIndexX->setValue(0);
    ui->boxObjAbsIndexY->setValue(0);
    ui->boxObjAbsIndexZ->setValue(0);

}
