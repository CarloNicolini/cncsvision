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

#ifndef MATRIXPAINTWIDGET_H
#define MATRIXPAINTWIDGET_H

#include <QWidget>
#include <Eigen/Core>


using namespace Eigen;

class QImage;
class MainWindow;

class MatrixPaintWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MatrixPaintWidget(QWidget *parent = 0);
    void setMainWindow(MainWindow *m){ mainWindow=m;};

signals:
    
public slots:
    void updatePixmap(const QImage &image);
    void paintEvent(QPaintEvent *event);
    void updateGeometry();
    void showContextMenu(const QPoint& pos); // this is a slot
    QPixmap getCurrentPixmap();
private:
    MainWindow *mainWindow;
    QPixmap pixmap,pixmapOrig;
    void resizeEvent(QEvent *event);
//    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);

//    void mouseMoveEvent(QMouseEvent *event);
//    void mouseReleaseEvent(QMouseEvent *event);

};

#endif // MATRIXPAINTWIDGET_H
