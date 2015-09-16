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


#include "matrixpaintwidget.h"
#include <QMessageBox>
#include <QPaintEvent>
#include <QPainter>
#include <QFrame>
#include <QImage>
#include <iostream>
#include <QRgb>
#include <Eigen/Core>
#include <iostream>
#include <QFileDialog>
#include <QMenu>
#include "Util.h"
#include "mainwindow.h"

using namespace std;
using namespace Eigen;

MatrixPaintWidget::MatrixPaintWidget(QWidget *parent) :
        QWidget(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(showContextMenu(const QPoint&)));
    this->setWindowTitle("Output matrix");
}

void MatrixPaintWidget::updatePixmap(const QImage &image)
{
    resize(image.width()*5,image.height()*5);
    pixmapOrig = QPixmap::fromImage(image.scaled(this->width(),this->width()));
    pixmap = pixmapOrig;
   update();
}

void MatrixPaintWidget::resizeEvent(QEvent *event)
{

}

void MatrixPaintWidget::paintEvent( QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (pixmap.isNull())
    {
        painter.fillRect(event->rect(),Qt::black);
        painter.setPen(Qt::white);
        painter.drawText(rect(), Qt::AlignCenter, "No model to show, press Run...");
        return;
    }
    else
    {
        painter.drawPixmap(0,0,pixmap);
    }
    updateGeometry();
}

void MatrixPaintWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton )
        showContextMenu(event->pos());
    //if ( event->button() == Qt::LeftButton )
    //mainWindow->compute();
}

void MatrixPaintWidget::wheelEvent(QWheelEvent *event)
{
    // da implementare lo zoom cout << event->delta() << endl;
}


void MatrixPaintWidget::updateGeometry()
{
    pixmap = pixmapOrig.scaled(this->width(),this->height(),Qt::KeepAspectRatio);
}

void MatrixPaintWidget::showContextMenu(const QPoint& pos) // this is a slot
{
    // for most widgets
    if ( !pixmap.isNull() )
    {
    QPoint globalPos = this->mapToGlobal(pos);
    QMenu myMenu;
    myMenu.addAction("Save image as bitmap...");
    myMenu.addAction("Save as raw txt data..");
    myMenu.addAction("Save as this matrix parameters data..");

    QAction* selectedItem = myMenu.exec(globalPos);

  if ( selectedItem == myMenu.actions().at(0) )
  {
      QString filename = QFileDialog::getSaveFileName(this,"Save matrix image file...",QDir::currentPath(),".bmp");
      QImage image = pixmap.toImage();
      image.save(filename,"png",100);

      QMessageBox msgBox;
      msgBox.setText(QString("Image has been saved successfully to ").append(filename));
      msgBox.setStandardButtons(QMessageBox::Ok);
      msgBox.exec();
  }

  if ( selectedItem == myMenu.actions().at(1) )
  {
      QString filename = QFileDialog::getSaveFileName(this,"Save txt matrix file...",QDir::currentPath(),".txt");
      ofstream output;
      output.open(filename.toStdString().c_str());
      output << mainWindow->Pgxgy << endl;
      output.close();

      QMessageBox msgBox;
      msgBox.setText(QString("Matrix has been saved successfully to ").append(filename));
      msgBox.setStandardButtons(QMessageBox::Ok);
      msgBox.exec();
  }

  if ( selectedItem == myMenu.actions().at(2) )
  {
     mainWindow->on_actionSave_parameters_file_triggered();
  }
}
}

QPixmap MatrixPaintWidget::getCurrentPixmap()
{
    return pixmap;
}
