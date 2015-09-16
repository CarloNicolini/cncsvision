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

#ifndef ModelThread_H
#define ModelThread_H

#include <QMutex>
#include <QSize>
#include <QThread>
#include <QWaitCondition>
#include "mainwindow.h"

#include <Eigen/Core>

// CNCSVision includes
#include "ParametersLoader.h"
#include "Util.h"
#include "BayesNetwork.h"
#include "Inferencer.h"


QT_BEGIN_NAMESPACE
class QImage;
QT_END_NAMESPACE

//! [0]
class ModelThread : public QThread
{
    Q_OBJECT
public:
    ModelThread(QObject *parent = 0);
    ~ModelThread();
    void requestStop(bool val);

signals:
    void theImage(const QImage &image);
    int rowPercent(int i);
protected:
    void run();

private:

    uint doubleToJetMap(double x);

    QImage image;
    MainWindow *p;

    QMutex mutex;
    bool stopRequested;
   // QWaitCondition condition;
};
//! [0]

#endif
