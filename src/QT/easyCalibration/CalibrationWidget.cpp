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

#include <QtGui>
#include <QtOpenGL>
#include "CalibrationWidget.h"
#ifdef __APPLE__
QString CNCSVISION_BASE_DIRECTORY("/Users/rs/workspace/");
#endif

#ifdef __linux__
//QString CNCSVISION_BASE_DIRECTORY("/home/carlo/workspace/");
#endif

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

/**
 * @brief ShaderWidget::ShaderWidget
 * @param parent
 * @param shareWidget
 */
CalibrationWidget::CalibrationWidget(QWidget *parent) :
    QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{

    // Get the 2D points and 3D points
    this->setAutoBufferSwap(true);
    this->setMouseTracking(true);
    this->setAutoFillBackground(false);
    this->setCursor(Qt::BlankCursor);
    this->resize(PROJECTOR_WIDTH,PROJECTOR_HEIGHT);
    glPointSize(0.1);
    QTimer *timer = new QTimer(this);
    timer->start(15);
    QObject::connect(timer,SIGNAL(timeout()),this,SLOT(repaint()));
    this->setFocus();


    drawingText=true;
}

void CalibrationWidget::moveCursor(int x, int y)
{
	this->lastPoint.setX(this->lastPoint.x()+x);
	this->lastPoint.setY(this->lastPoint.y()+y);
}

/**
 * @brief ShaderWidget::~ShaderWidget
 */
CalibrationWidget::~CalibrationWidget()
{
}

/**
 * @brief ShaderWidget::minimumSizeHint
 * @return
 */
QSize CalibrationWidget::minimumSizeHint() const
{
    return QSize(PROJECTOR_WIDTH, PROJECTOR_HEIGHT);
}


/**
 * @brief ShaderWidget::sizeHint
 * @return
 */
QSize CalibrationWidget::sizeHint() const
{
    return QSize(PROJECTOR_WIDTH, PROJECTOR_HEIGHT);
}

/**
 * @brief ShaderWidget::initializeGL
 */
void CalibrationWidget::initializeGL()
{
    glEnable(GL_MULTISAMPLE);
    qglClearColor(Qt::black);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT  );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/**
 * @brief ShaderWidget::saveData
 */
void CalibrationWidget::saveData()
{
    QString filename = QFileDialog::getSaveFileName(this,"Select points output file name", QDir::currentPath());
    QFileInfo outputfile(filename);

	filename.replace("."+outputfile.suffix(),"");
	QString pointsOutputFileName = filename + ".txt";
    QString imageOutputFileName  = filename + ".bmp";

	QImage frame = this->grabFrameBuffer();

    if ( !frame.save(imageOutputFileName,NULL,-1) )
	{
		QMessageBox::warning(this,"Error saving image","Can't save image");
	}
	
	QFile outputFile(pointsOutputFileName);
	outputFile.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream out(&outputFile);

    //outputPoints.open(pointsOutputFileName.toStdString().c_str());
    out << "# Calibration points" << endl;
    for (int i=0; i<points2D.size();i++)
    {
        out << points2D.at(i).x() << "\t" << points2D.at(i).y() << endl;
    }
}

/**
 * @brief CalibrationWidget::mouseMoveEvent
 * @param event
 */
void CalibrationWidget::mouseMoveEvent(QMouseEvent *event)
{
    lastPoint = QPoint(event->x() ,event->y());
}

/**
 * @brief CalibrationWidget::mouseMoveEvent
 * @param event
 */
void CalibrationWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton )
    {
        // Search if the point already exists remove it
        lastPoint = QPoint(event->x() ,event->y());
		addPoint();
    }
}

/**
 * @brief ShaderWidget::paintGL
 */
void CalibrationWidget::paintGL()
{
    //makeCurrent();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    qglClearColor(Qt::black);
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glShadeModel(GL_FLAT);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::green);
    painter.drawPoint(lastPoint);
    painter.setPen(Qt::red);
    for (int i=0; i< this->points2D.size();i++)
    {
        painter.setPen(Qt::red);
        painter.drawPoint(points2D.at(i));
    }

	painter.drawPoint(PROJECTOR_WIDTH/2,PROJECTOR_HEIGHT/2);
		
    if ( drawingText )
    {
        painter.setPen(Qt::white);
		painter.drawText(40,PROJECTOR_HEIGHT-20,QString("(x,y)=(")+ QString::number(lastPoint.x())+","+QString::number(lastPoint.y())+")");
        //painter.drawText(80,PROJECTOR_HEIGHT-20,QString::number(lastPoint.y()));
        painter.drawText(120,PROJECTOR_HEIGHT-40,"Press F to toggle Fullscreen");
        painter.drawText(120,PROJECTOR_HEIGHT-60,"Press S to select the output file name");
        painter.drawText(120,PROJECTOR_HEIGHT-80,"Press Q to quit and save");
        painter.drawText(120,PROJECTOR_HEIGHT-100,"RightMouse to save/erase points");
        painter.drawText(120,PROJECTOR_HEIGHT-120,"Press T to toggle this text");
		painter.drawText(120,PROJECTOR_HEIGHT-140,"Press R to save this point");

        QRect rect;
        QPen pen; pen.setWidth(1);pen.setColor(Qt::white);
        painter.setPen(pen);
        rect.setTopLeft(QPoint(0,0));
        rect.setBottomRight(QPoint(PROJECTOR_WIDTH-1,PROJECTOR_HEIGHT-1));
        painter.drawRect(rect);
    }
    painter.end();
}

void CalibrationWidget::toggleText()
{
    this->drawingText = !this->drawingText;
}

/**
 * @brief ShaderWidget::resizeGL
 * @param width
 * @param height
 */
void CalibrationWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, (GLsizei) width, (GLsizei) height); // Set our viewport to the size of our window
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float FOV=90;
    gluPerspective(FOV, (float)width / (float)height, 0.1, 200.0);
    glMatrixMode(GL_MODELVIEW);
}

void CalibrationWidget::addPoint()
{
	for (int i=0; i<points2D.size();i++)
    {
		if (lastPoint == points2D.at(i))
        {
                points2D.remove(i);
                qDebug() << "Removed  " << lastPoint ;
                return;
            }
        }
      qDebug() << "Added " << lastPoint ;
      this->points2D.push_back(lastPoint);
}
