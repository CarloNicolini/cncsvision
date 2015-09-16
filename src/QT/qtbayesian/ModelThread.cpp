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
#include <QImage>
#include <math.h>

#include <iostream>
using std::cerr;
using std::endl;

#include "ModelThread.h"
#include "matrixpaintwidget.h"

ModelThread::ModelThread(QObject *parent)
    : QThread(parent)
{
    p = (MainWindow*)parent;
}
ModelThread::~ModelThread()
{

}

uint ModelThread::doubleToJetMap(double x)
{
    double  r = 255*((x >= 0.375 & x < 0.625) * (4.0 * x - 1.5) + (x >= 0.625 & x < 0.875) + (x >= 0.875) * (-4.0 * x + 4.5));
    double g = 255*((x >= 0.125 & x < 0.375) * (4.0 * x - 0.5) + (x >= 0.375 & x < 0.625) + (x >= 0.625 & x < 0.875) * (-4.0 * x + 3.5));
    double b = 255*((x < 0.125) * (4.0 * x + 0.5) + (x >= 0.125 & x < 0.375) + (x >= 0.375 & x < 0.625) * (-4.0 * x + 2.5));

    return qRgb((int)r,(int)g,(int)b);
}

void ModelThread::requestStop(bool val)
{
    //QMutexLocker lock(&mutex);
    stopRequested=val;
}

void ModelThread::run()
{
    QImage image(p->granularityGx, p->granularityGy,QImage::Format_RGB32);
    unsigned int granularity = floor(p->dgranularity+0.5);
    p->Pgxgy.setZero(p->granularityGx, p->granularityGy);

    for (int i=0; i<p->granularityGx; i++)
    {
         if ( !stopRequested )
        {
        QMutexLocker lock(&mutex);

        #pragma omp parallel for ordered schedule(dynamic)
        for (int j=0; j<p->granularityGy; j++)
        {
            // A new instance of a multidimensional probability density function to integrate
            DBayesianModel dbayes(p->nDimensions);
            // Set the parameters of the pdf (see Fulvio for details)
            dbayes.setSigmaG(p->sigmaGx,p->sigmaGy);
            dbayes.setSigmaOmega(p->sigmaOmegax,p->sigmaOmegay,p->sigmaOmegaz);
            dbayes.setSigmaOmega0(p->sigmaOmega0x,p->sigmaOmega0y,p->sigmaOmega0z);
            dbayes.setAMean(p->ma1,p->ma2,p->ma3,p->ma4,p->ma5,p->ma6);
            dbayes.setOmega0Mean( p->mOmega0x,p->mOmega0y,p->mOmega0z);
            dbayes.setOmegaMean( p->mOmegax,p->mOmegay,p->mOmegaz);
            dbayes.setEpsilons(p->epsilon1,p->epsilon2,p->epsilon3,p->epsilon4,p->epsilon5,p->epsilon6);

	    double gx = i*p->gStep+p->gxMin;
	    double gy = j*p->gStep+p->gyMin;
	    dbayes.setGxGy(gx,gy);
	    dbayes.finalize();
	     Inferencer inferencer;
	    // Inferencer for Gibbs Sampling
	    inferencer.init( &dbayes, p->nDimensions,granularity, p->domainStep, p->domain );
	    p->Pgxgy(j,i) =  inferencer.gibbsSamplingIntegral(p->nGibbsBurnIn,p->nGibbsValid);;
	}
	double percentCompleted = (double)(i+1)/(double)p->granularityGx*100.0;
	emit  rowPercent((int)percentCompleted);
	}
	else
	{
	    int w = p->matrixWidget->width()/5;
	    int h = p->matrixWidget->height()/5;
	    QImage tmp(w,h,QImage::Format_RGB32);
	    QPainter painter;
	    painter.begin(&tmp);
	    painter.fillRect(tmp.rect(),Qt::black);
	    painter.end();
	    emit theImage(tmp);
	    return;
	}
    }

    double maxCoeff = p->Pgxgy.maxCoeff();
    if ( std::fabs(maxCoeff) < std::numeric_limits<double>::epsilon())
        p->Pgxgy.setZero(p->granularityGx,p->granularityGy);
    else
        p->Pgxgy/=maxCoeff;

    for (int i=0; i<p->granularityGx; i++)
    {
        #pragma omp parallel for ordered schedule(dynamic)
        for (int j=0; j<p->granularityGy; j++)
            image.setPixel(i,j,doubleToJetMap(p->Pgxgy.coeffRef(i,j)));
    }
    emit theImage(image);
}
