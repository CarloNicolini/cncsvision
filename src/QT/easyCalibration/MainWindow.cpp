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
	switchUpDown=swichLeftRight=false;
    ui->setupUi(this);
    //this->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint );
    //this->setWindowState(Qt::WindowFullScreen);
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
    switch (e->key())
    {
	case Qt::Key_PageUp:
	{
		this->switchUpDown=!this->switchUpDown;
		break;
	}
	case Qt::Key_PageDown:
	{
		this->swichLeftRight=!this->swichLeftRight;
		break;
	}
	case Qt::Key_T:
    {
        ui->calibrationWidget->toggleText();
        break;
    }
	case Qt::Key_Up:
	{
		if ( switchUpDown )
			ui->calibrationWidget->moveCursor(0,-1);
		else
			ui->calibrationWidget->moveCursor(0,1);
		
		break;
	}
	case Qt::Key_Down:
	{
		if ( switchUpDown )
			ui->calibrationWidget->moveCursor(0,1);
		else
			ui->calibrationWidget->moveCursor(0,-1);
		break;
	}
	case Qt::Key_Right:
	{
		if ( swichLeftRight )
			ui->calibrationWidget->moveCursor(1,0);
		else
			ui->calibrationWidget->moveCursor(-1,0);
		break;
	}
	case Qt::Key_Left:
	{
		if ( swichLeftRight )
			ui->calibrationWidget->moveCursor(-1,0);
		else
			ui->calibrationWidget->moveCursor(1,0);
		break;
	}
	case Qt::Key_R:
	{
		ui->calibrationWidget->addPoint();
			break;
	}
	case Qt::Key_F:
    {
        if (this->isFullScreen())
            setWindowState(Qt::WindowNoState);
        else
            setWindowState(Qt::WindowFullScreen);
    }
        break;
    case Qt::Key_S:
    {
        ui->calibrationWidget->saveData();
        break;
    }
    case  Qt::Key_Q:
    case Qt::Key_Escape :
    {
        ui->calibrationWidget->saveData();
        QApplication::quit();
        break;
    }
    }
}

