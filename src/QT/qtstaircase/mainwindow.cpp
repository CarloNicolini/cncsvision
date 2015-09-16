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

#include "mainwindow.h"
#include "ui_mainwindow.h"

// Standard includes
#include <iostream>
#include <fstream>
#include <numeric>

// CNCSVision includes
#include "ParametersLoader.h"
#include "Util.h"
#include "Mathcommon.h"

// QTStaircase includes
#include "TabStaircase.h"
// QCustomPlot
#include "QCustomPlot.h"

#ifdef _WIN32
#include <boost/math/special_functions.hpp>
using boost::math::erf;
#endif

#include <Eigen/Core>

using namespace util;
using std::fstream;
using std::ifstream;
using namespace Eigen;
MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Set the tabWidget as the main widget of the application
    setCentralWidget(ui->tabWidget);
    // Connect the spinbox to the number of staircases tabs widgets
    connect(ui->spinBoxNStaircases,SIGNAL(valueChanged(int)),
            ui->tabStaircase,SLOT(setNumberOfTabs(int)));

    //    ui->tabStaircase->setNumberOfTabs(ui->spinBoxNStaircases->value());

    list << "ID" << "Ntrial" << "Steps" << "Inversions" << "State" << "Response" ;
    ui->tableWidget->setColumnCount(6);
    //ui->tableWidget->setRowCount(1);
    ui->tableWidget->setHorizontalHeaderLabels(list);
    //this->resize(this->sizeHint());

    ui->tabStaircase->setNumberOfTabs(1);
    this->setupGraphThings();
    this->resize(this->minimumSizeHint());
    ui->tabStaircase->resize(ui->tabStaircase->minimumSizeHint());
    connect(ui->comboBoxDrawStyle,SIGNAL(currentIndexChanged(int)),this,SLOT(update()));
    //connect(ui->lineEditStimCorrectAns,SIGNAL(textChanged(QString)),ui->pushButton,SLOT(setWindowTitle(QString)));
    //ui->tabWidget->di
    this->resize(0,0);
}

void MainWindow::setupGraphThings()
{
    // Fill the QVector of colors used to display the consecutive graphic lines elements
    // It uses the same order of gnuplot
    colors.push_back(Qt::red);
    colors.push_back(Qt::green);
    colors.push_back(Qt::blue);
    colors.push_back(Qt::magenta);
    colors.push_back(Qt::cyan);
    colors.push_back(Qt::darkRed);
    colors.push_back(Qt::darkGreen);
    colors.push_back(Qt::darkBlue);
    colors.push_back(Qt::darkMagenta);
    colors.push_back(Qt::darkCyan);
    colors.push_back(Qt::darkMagenta);
    colors.push_back(Qt::yellow);
    colors.push_back(Qt::darkYellow);
    colors.push_back(Qt::gray);
    colors.push_back( Qt::darkGray);
    colors.push_back(Qt::lightGray);

    // Here a cast is necessary to work with the promoted widget
    QCustomPlot *customPlot = dynamic_cast<QCustomPlot*>(ui->customPlot);
    customPlot->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom)); // period as decimal separator and comma as thousand separator
    customPlot->setAntialiasedElement(QCustomPlot::aeScatters, true);

    connect(ui->spinBoxNStaircases,SIGNAL(valueChanged(int)),customPlot,SLOT(setNumberOfGraphs(int)));

    customPlot->legend->setVisible(true);
    QFont legendFont = font();  // start out with MainWindow's font..
    legendFont.setPointSize(9); // and make a bit smaller for legend
    customPlot->legend->setFont(legendFont);
    customPlot->legend->setPositionStyle(QCPLegend::psBottomRight);
    customPlot->legend->setBrush(QBrush(QColor(255,255,255,230)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionQuit_triggered()
{
    this->close();
}

void MainWindow::on_actionOpenFile_triggered()
{
    //    ui->tabStaircase->reset();
    QFileDialog dialog;
    QString filename = dialog.getOpenFileName(this,"Parameters file to open","../libs/experiment/", "Text file (*.txt);;Exp files (*.exp)");
    if ( filename.isEmpty() )
        return;

    // Here open a parameters file
    ParametersLoader params;
    params.loadParameterFile(filename.toStdString());

    int nstaircases = util::str2num<int>(params.find("sStairNumber"));

    ui->tabStaircase->setNumberOfTabs(nstaircases);

    ui->spinBoxNStaircases->setValue(nstaircases);
    // Separate handling of positive steps sizes
    string content = params.find("sStairPositiveStep");
    vector<string> stairstepsstrings;
    util::tokenizeString(content,stairstepsstrings,";");

    if ( QString(params.find("sStairCorrectAnswers").c_str()).count(",") != nstaircases-1 )
    {   QMessageBox::warning( this,  "Error",  "Wrong number of correct answers!\nCheck \"sStairCorrectAnswers\" to have "+QString::number(nstaircases)+" elements" );
        return;
    }

    if ( QString(params.find("sStairStartStates").c_str()).count(",") != nstaircases-1 )
    {
        QMessageBox::warning( this,  "Error",  "Wrong number of start states!\nCheck \"sStairStartStates\" to have "+QString::number(nstaircases)+" elements" );
        return;
    }

    if ( QString(params.find("sStairAscending").c_str()).count(",") != nstaircases-1 )
    {
        QMessageBox::warning( this,  "Error",  "Wrong number of ascending/descending staircases!\nCheck \"sStairAscending\" to have "+QString::number(nstaircases)+" elements" );
        return;
    }

    if ( QString(params.find("sStairMaxInversions").c_str()).count(",") != nstaircases-1 )
    {
        QMessageBox::warning( this,  "Error",  "Wrong number of max inversions!\nCheck \"sStairMaxInversions\" to have "+QString::number(nstaircases)+" elements" );
        return;
    }

    if ( QString(params.find("sStairMaxTrials").c_str()).count(",") != nstaircases-1 )
    {
        QMessageBox::warning( this,  "Error",  "Wrong number of max trials!\nCheck \"sStairMaxTrials\" to have "+QString::number(nstaircases)+" elements" );
        return;
    }

    if ( QString(params.find("sStairNegativeStep").c_str()).count(",") != 0 )
    {
        QMessageBox::warning( this,  "Error",  "You can only use one negative step size for every staircase! Please modify the parameters file under sStairNegativeStep section." );
        return;
    }

    if ( QString(params.find("sStairClampLower").c_str()).count(",") != 0 )
    {
        QMessageBox::warning( this,  "Error",  "Please specify only 1 number for lower clamp. It will be applied to every staircase." );
        return;
    }

    if ( QString(params.find("sStairClampUpper").c_str()).count(",") != 0 )
    {
        QMessageBox::warning( this,  "Error",  "Error",  "Please specify only 1 number for upper clamp. It will be applied to every staircase." );
        return;
    }

    if ( QString(params.find("sStairMaxClampHits").c_str()).count(",") != 0 )
    {
        QMessageBox::warning( this,  "Error",  "Please specify only 1 number for maximum clamp hits. It will be applied to every staircase." );
        return;
    }

    //cerr << "COUNT= " << ui->tabStaircase->count() << " " << ui->tabStaircase->getNumberOfTabs() << endl;
    for (int i=0; i<ui->tabStaircase->count(); i++)
    {
        StaircaseParamSetter *s = ui->tabStaircase->getStaircaseParamsSetter(i);
        s->setStartState(util::str2num<double>(params.find("sStairStartStates"),",").at(i));
        s->setCorrectAnswers(util::str2num<int>(params.find("sStairCorrectAnswers"),",").at(i));
        s->setAscendingStaircase(util::str2num<bool>(params.find("sStairAscending"),",").at(i));
        s->setMaxReversals(util::str2num<int>(params.find("sStairMaxInversions"),",").at(i));
        s->setMaxTrials(util::str2num<int>(params.find("sStairMaxTrials"),",").at(i));
        s->setNegativeStep(util::str2num<double>(params.find("sStairNegativeStep")));

        if ( content.find(";")!=string::npos )
            s->setPositiveSteps(stairstepsstrings.at(i));
        else
            s->setPositiveSteps(stairstepsstrings.at(0));
    }

    if ( params.exists("sStairClampLower") || params.exists("sStairClampUpper") || params.exists("sStairMaxClampHits"))
    {
        for (int i=0; i<ui->tabStaircase->count(); i++)
        {
            StaircaseParamSetter *s = ui->tabStaircase->getStaircaseParamsSetter(i);
            s->setClampActive(true);
            s->setUpperClamp(util::str2num<double>(params.find("sStairClampUpper")));
            s->setLowerClamp(util::str2num<double>(params.find("sStairClampLower")));
            s->setMaxClampHits(util::str2num<double>(params.find("sStairMaxClampHits")));
        }
    }

    if (!params.exists("GuessRate"))
    {   QMessageBox::warning( this,  "Error",  "Parameter \"GuessRate\" doesn't exists. Setting it to 0.0" );
        ui->doubleSpinBoxGuessRate->setValue(0.0);
    }
    else
        ui->doubleSpinBoxGuessRate->setValue(util::str2num<double>(params.find("GuessRate")));

    if (!params.exists("LapseRate"))
    {   QMessageBox::warning( this,  "Error",  "Parameter \"LapseRate\" doesn't exists. Setting it to 0.0" );
        ui->doubleSpinBoxLapseRate->setValue(0.0);
    }
    else
        ui->doubleSpinBoxLapseRate->setValue(util::str2num<double>(params.find("GuessRate")));

    if (!params.exists("Mu"))
    {   QMessageBox::warning( this,  "Error",  "Parameter \"Mu\" doesn't exists. Setting \"Mean\" to 0.0" );
        ui->doubleSpinBoxMean->setValue(0.0);
    }
    else
        ui->doubleSpinBoxMean->setValue(util::str2num<double>(params.find("Mu")));

    if (!params.exists("Sigma"))
    {   QMessageBox::warning( this,  "Error",  "Parameter \"Sigma\" doesn't exists. Setting \"standard deviation\" it to 1" );
        ui->doubleSpinBoxStd->setValue(1.0);
    }
    else
        ui->doubleSpinBoxStd->setValue(util::str2num<double>(params.find("Sigma")));

    if (!params.exists("sStairClampLower"))
    {   QMessageBox::warning( this,  "Error",  "Parameter \"sStairClampLower doesn't exists. Setting it to default");
    }
    else
    {
        double lowerclamp = util::str2num<double>(params.find("sStairClampLower"));
        for (int i=0; i<ui->tabStaircase->count();i++)
         ui->tabStaircase->getStaircaseParamsSetter(i)->setLowerClamp(lowerclamp);
    }

    if (!params.exists("sStairClampUpper"))
    {   QMessageBox::warning( this,  "Error",  "Parameter \"sStairClampLower doesn't exists. Setting it to default");
    }
    else
    {

        double upperclamp = util::str2num<double>(params.find("sStairClampUpper"));
        for (int i=0; i<ui->tabStaircase->count();i++)
            ui->tabStaircase->getStaircaseParamsSetter(i)->setUpperClamp(upperclamp );
    }

    if (!params.exists("sStairMaxClampHits"))
    {   QMessageBox::warning( this,  "Error",  "Parameter \"sStairMaxClampHits doesn't exists. Setting it to default");
    }
    else
    {
        int maxclamphits= util::str2num<double>(params.find("sStairMaxClampHits"));
        for (int i=0; i<ui->tabStaircase->count();i++)
            ui->tabStaircase->getStaircaseParamsSetter(i)->setMaxClampHits(maxclamphits);
    }

}

void MainWindow::initParStaircase()
{
    std::vector<bool> ascending;
    std::vector<double> startStates,negativeStep;
    std::vector<int> maxInversions,maxTrials,correctAnswers;
    std::vector< vector<double> > stairSteps;

    parStaircase.init(ui->tabStaircase->count());
    for (int i=0; i<ui->tabStaircase->count(); i++)
    {
        StaircaseParamSetter *s = ui->tabStaircase->getStaircaseParamsSetter(i);
        ascending.push_back(s->getAscendingStaircase() );
        startStates.push_back(s->getStartState());
        maxInversions.push_back(s->getMaxReversals());
        maxTrials.push_back(s->getMaxTrials());
        correctAnswers.push_back(s->getCorrectAnswers());
        negativeStep.push_back(s->getNegativeStep());
        stairSteps.push_back(s->getPositiveSteps());
    }

      for (int i=0; i<ui->tabStaircase->count(); i++)
      {
          cout << ascending.at(i) << " " << startStates.at(i) << " " << maxInversions.at(i) << " " << correctAnswers.at(i) << " " << stairSteps.at(i) <<endl;
      }

    parStaircase.setAscending(ascending);
    parStaircase.setStartState(startStates);
    parStaircase.setCorrectAnswers(correctAnswers);
    parStaircase.setMaxReversals(maxInversions);
    parStaircase.setMaxTrials(maxTrials);
    // Set different steps for every different staircase
    for (int i=0; i<ui->tabStaircase->count(); i++)
    {
        parStaircase.setStairStep(i,stairSteps.at(i),negativeStep.at(i));
    }
    // Set clamp values for every different staircase
    for (int i=0; i<ui->tabStaircase->count(); i++)
    {
        StaircaseParamSetter *s = ui->tabStaircase->getStaircaseParamsSetter(i);
        if ( s->isClampActive())
            parStaircase.setClamp(i,s->getLowerClamp(),s->getUpperClamp(),s->getMaxClampHits() );
    }
}

void MainWindow::on_buttonStart_clicked()
{
    dynamic_cast<QCustomPlot*>(ui->customPlot)->clearPlottables();
    dynamic_cast<QCustomPlot*>(ui->customPlot)->clearGraphs();
    ui->tableWidget->clear();
    //ui->tabStaircase->reset();
    lastRunNStaircase=ui->tabStaircase->count();
    //ui->tabStaircase->setNumberOfTabs(ui->spinBoxNStaircases->value());
    ui->spinBoxLastInvAverage->setEnabled(true);
    ui->spinBoxLastInvAverage->setValue(1);

    id2inversions2states.clear();
    simulatedResponses.clear();
    simulatedIDS.clear();
    simulatedNInversions.clear();
    simulatedNTrial.clear();
    simulatedStaircaseSteps.clear();
    simulatedStates.clear();

    double mean=ui->doubleSpinBoxMean->value();
    double std=ui->doubleSpinBoxStd->value();
    double guessRate = ui->doubleSpinBoxGuessRate->value();
    double lapseRate =  ui->doubleSpinBoxLapseRate->value();

    ui->statusbar->showMessage(QString("Computing..."));
    ui->tableWidget->setEnabled(false);

    for (int i=0; i< ui->spinBoxSimRepetitions->value(); i++ )
    {
        initParStaircase();
        int finished=0;
        int trial=0;
        while ( finished==0 )
        {
            Staircase *s = parStaircase.getCurrentStaircase();
            double decision = mathcommon::unifRand(0.0,1.0);
            double x = s->getState();
            double observer  = guessRate+(1-guessRate-lapseRate)*(0.5*(1.0+erf( (x-mean)/( sqrt(2.0)*std ))));
            bool response = decision < observer;
            // Store the values in these std::vectors
            /*
            simulatedIDS.push_back(s->getID());
            simulatedNInversions.push_back(s->getInversions());
            simulatedStates.push_back(s->getState());
            simulatedResponses.push_back(response);
            simulatedStaircaseSteps.push_back(s->getStepsDone());
            simulatedNTrial.push_back(trial++);
            //cerr << "Pushing " << s->getID() << " " << s->getInversions() << " with state " << s->getState() << endl;
            id2inversions2states[s->getID()][s->getInversions()]=s->getState();
            */
            finished = parStaircase.step(response);
        }

    }


    ui->statusbar->showMessage(QString("Updating table..."));
    ui->tableWidget->setEnabled(true);
    updateTable();
    ui->statusbar->showMessage(QString("Updating graph..."));
    updateGraph(lastRunNStaircase);
    ui->statusbar->showMessage(QString("Data updated"));
    ui->buttonStart->setDown(false);

}

void MainWindow::updateTable()
{
    ui->tableWidget->clearContents();
    ui->tableWidget->clear();
    ui->tableWidget->reset();

    int nrows = (int)simulatedIDS.size();

    ui->tableWidget->setColumnCount(6);
    ui->tableWidget->setRowCount(nrows);
    ui->tableWidget->setHorizontalHeaderLabels(list);

    id2string.clear();
    for (int i=0; i<ui->tabStaircase->count(); i++)
    {
        id2string[i]=ui->tabStaircase->getStaircaseParamsSetter(i)->getStaircaseName();
    }

    for (int i=0; i<nrows; i++)
    {
        QTableWidgetItem *idItem = new QTableWidgetItem();
        QTableWidgetItem *nInvItem = new QTableWidgetItem();
        QTableWidgetItem *statesItem = new QTableWidgetItem();
        QTableWidgetItem *respItem = new QTableWidgetItem();
        QTableWidgetItem *stepsItem = new QTableWidgetItem();
        QTableWidgetItem *ntrialItem = new QTableWidgetItem();

        idItem->setText(id2string[simulatedIDS.at(i)]);
        ntrialItem->setText(QString::number(simulatedNTrial.at(i)));
        nInvItem->setText(QString::number(simulatedNInversions.at(i)));
        statesItem->setText(QString::number(simulatedStates.at(i),'f',3));
        respItem->setText(QString::number(simulatedResponses.at(i)));
        stepsItem->setText(QString::number(simulatedStaircaseSteps.at(i)));

        ui->tableWidget->setItem(i,0,idItem);
        ui->tableWidget->setItem(i,1,ntrialItem);
        ui->tableWidget->setItem(i,2,stepsItem);
        ui->tableWidget->setItem(i,3,nInvItem);
        ui->tableWidget->setItem(i,4,statesItem);
        ui->tableWidget->setItem(i,5,respItem);
    }

    if (ui->checkBoxSortByID->isChecked())
        ui->tableWidget->sortByColumn(0,Qt::AscendingOrder);

    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->resizeRowsToContents();
    ui->labelNTrialsRequired->setText(QString::number(nrows));
    updateStats();
}

void MainWindow::updateGraph(int n, QCPGraph::ScatterStyle plotStyle)
{
    QCustomPlot *customPlot = dynamic_cast<QCustomPlot*>(ui->customPlot);
    customPlot->clearPlottables();
    customPlot->clearGraphs();

    int totalgraphs  =  customPlot->graphCount();

    for (int i=totalgraphs-1; i>=0; i--)
        customPlot->removeGraph(i);
    for (int i=0; i<n; i++)
        customPlot->addGraph();

    QTableWidget *src = ui->tableWidget;
    if ( src->rowCount()==1 || (customPlot->graphCount()==0) )
        return;
    double minval=std::numeric_limits<int>::max(),maxval=std::numeric_limits<int>::min();
    int maxsteps=std::numeric_limits<int>::min();
    int rows  =src->rowCount();
    QPen pen;

    for (int i=0; i<rows; i++)
    {
        // Qua mettere string2id
        int id = src->item(i,0)->text().toInt();
        int stairTrial = src->item(i,2)->text().toInt();
        double value = src->item(i,4)->text().toDouble();
        minval = std::min(value,minval);
        maxval = std::max(value,maxval);
        maxsteps = std::max(stairTrial,maxsteps);
        pen.setColor(colors.at(id));
        customPlot->graph(id)->setName("Staircase " + id2string[id]);
        customPlot->graph(id)->setPen(pen);
        customPlot->graph(id)->setScatterStyle(plotStyle);
        customPlot->graph(id)->addData(stairTrial,value);
    }

    // give the axes some labels:
    QCPLegend legend(customPlot);
    legend.setVisible(true);
    customPlot->xAxis->setLabel("Trials");
    customPlot->yAxis->setLabel("Stimulus intensity");
    // set axes ranges, so we see all data:
    customPlot->xAxis->setRange(0, maxsteps);
    customPlot->yAxis->setRange(minval,maxval);

    // Now draw the psychometric function on yAxis2
    if (drawPsychometric)
    {
        double mean=ui->doubleSpinBoxMean->value();
        double std=ui->doubleSpinBoxStd->value();
        double guessRate=ui->doubleSpinBoxGuessRate->value();
        double lapseRate =ui->doubleSpinBoxLapseRate->value();
        double xbase = 0.9*customPlot->xAxis->range().upper;

        QCPGraph *psychograph = customPlot->addGraph();
        customPlot->xAxis2->setVisible(true);
        customPlot->yAxis2->setVisible(true);
        customPlot->yAxis2->setLabel("Psychometric function (dotted)");
        customPlot->xAxis2->setLabel("Probability (%)");
        customPlot->xAxis2->setAutoTickLabels(true);
        customPlot->xAxis2->setRange(0.0,1.0);


        QPen psychometricPen;
        psychometricPen.setWidth(2);
        psychometricPen.setStyle(Qt::DashLine);
        psychometricPen.setColor(Qt::black);

        psychograph->setName("Reference psychometric ("+QString::fromUtf8("μ ,") +QString::fromUtf8("σ")+")=("+QString::number(mean) +", " + QString::number(std) +" )");
        psychograph->setPen(psychometricPen);
        psychograph->setKeyAxis(customPlot->yAxis2);
        psychograph->setValueAxis(customPlot->xAxis2);

        connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
        connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

        customPlot->yAxis2->setRange(mean-10*std , mean+10*std);
        customPlot->xAxis2->setRange(0.0,6+maxsteps);

        customPlot->setupFullAxesBox();

        for (double x= mean-10*std ; x<= mean+10*std ; x+=std/50)
        {
            double target =  guessRate +(1.0-guessRate-lapseRate)*(0.5*(1.0+erf( -(x-mean)/( sqrt(2.0)*std ))));
            psychograph->addData(x, 6*target+maxsteps );
        }
    }
    else
    {
        customPlot->xAxis2->setVisible(false);
        customPlot->yAxis2->setVisible(false);
    }
    customPlot->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    customPlot->setRangeZoom( Qt::Vertical);
    customPlot->setAutoMargin(true);
    customPlot->replot();
}

void MainWindow::on_actionExportResultsToFile_triggered()
{
    QFileDialog dialog;
    dialog.setAcceptMode(QFileDialog::AcceptSave);//To save files,default is open mode.
    dialog.setNameFilter(".csv");
    QString filename = dialog.getSaveFileName(this,"Save simulation results...",QDir::currentPath(),".csv");

    std::ofstream output;
    output.open(filename.toStdString().c_str());
    output.precision(3);

    output << fixed << "ID\tNtrial\tSteps\tInversions\tState\tResponse" << endl ;
    int rows = ui->tableWidget->rowCount();
    int cols = ui->tableWidget->columnCount();
    for (int i=0; i<rows; i++)
    {
        for (int j=0; j<cols; j++)
            output << fixed << ui->tableWidget->item(i,j)->text().toStdString() << "\t";
        output << endl;
    }
}

void MainWindow::on_spinBoxLastInvAverage_valueChanged(int val)
{
    std::vector<int> x,minstaircase;
    for (int i=0; i<ui->tabStaircase->count(); i++)
        x.push_back(ui->tabStaircase->getStaircaseParamsSetter(i)->getMaxReversals());
    for (int i=0; i<ui->tabStaircase->count(); i++)
        if ( x.at(i) < val  )
            minstaircase.push_back(i);

    int revmin = *(std::min_element(x.begin(),x.end()));
    if (val > revmin )
    {
        string ret = util::stringify<vector<int> >(minstaircase);
        QMessageBox::warning(this,"Warning","You can't average on a number of inversion higher than the inversions for the staircase(s)\n"+QString(ret.c_str()));
        ui->spinBoxLastInvAverage->setValue(revmin);
    }
    updateStats();
}

void MainWindow::updateStats()
{
    ui->listWidget->clear();
    ui->listWidgetPsychoTarget->clear();

    double guessRate = ui->doubleSpinBoxGuessRate->value();
    double lapseRate = ui->doubleSpinBoxLapseRate->value();
    double mean = ui->doubleSpinBoxMean->value();
    double std = ui->doubleSpinBoxStd->value();

    if (id2inversions2states.empty())
        return;
    ui->listWidget->setEnabled(true);
    for (map<int,map<int,double> >::iterator iter = id2inversions2states.begin(); iter!=id2inversions2states.end(); ++iter)
    {
        double thisAverage=0.0;
        int j=0;
        for ( map<int,double>::reverse_iterator iter2 = iter->second.rbegin(); iter2!=iter->second.rend(); ++iter2 )
        {
            if ( j < ui->spinBoxLastInvAverage->value()   )
            {
                //cerr << "Adding  " << iter2->first << " " << iter2->second << endl;
                thisAverage+=iter2->second;
            }
            ++j;
        }
        thisAverage/=ui->spinBoxLastInvAverage->value();
        ui->listWidget->insertItem(iter->first, QString::number(thisAverage,'f',6) );
        double target =  guessRate +(1.0-guessRate-lapseRate)*(0.5*(1.0+erf( (thisAverage-mean)/( sqrt(2.0)*std ))));
        ui->listWidgetPsychoTarget->insertItem(iter->first, QString::number(target*100)+"\%" );
    }
}

void MainWindow::on_spinBoxNStaircases_valueChanged(int val)
{
    ui->spinBoxLastInvAverage->setDisabled( val!= lastRunNStaircase );
}

void MainWindow::on_actionNew_experiment_triggered()
{
    ui->tabStaircase->reset();
    ui->tabStaircase->setNumberOfTabs(1);
    ui->spinBoxNStaircases->setValue(1);
    updateTable();
    //updateGraph();
}

void MainWindow::on_comboBoxDrawStyle_currentIndexChanged(int index)
{
    switch (index)
    {
    case 0:
        plotStyle=QCPGraph::ssNone;
        break;
    case 1:
        plotStyle=QCPGraph::ssCross;
        break;
    case 2:
        plotStyle=QCPGraph::ssCircle;
        break;
    case 3:
        plotStyle=QCPGraph::ssSquare;
        break;
    }
    updateGraph(lastRunNStaircase,plotStyle);
}

void MainWindow::on_checkBox_stateChanged(int val)
{
    drawPsychometric=(bool)val;
    updateGraph(lastRunNStaircase,plotStyle);
}

void MainWindow::on_actionExport_graph_to_pdf_triggered()
{
    QCustomPlot *customPlot = dynamic_cast<QCustomPlot*>(ui->customPlot);
    if ( customPlot->graphCount()!=0 )
    {
        QFileDialog dialog;
        dialog.setAcceptMode(QFileDialog::AcceptSave);//To save files,default is open mode.
        QString filename = dialog.getSaveFileName(this,"Output file...",QDir::currentPath(),".pdf");
        customPlot->savePdf(filename);
    }
    else
    {
        QMessageBox::warning( this,  "Warning",  "No graph to export, please run experiment before." );
    }
}

void MainWindow::on_actionSaveFile_triggered()
{

    QFileDialog dialog;
    dialog.setAcceptMode(QFileDialog::AcceptSave);//To save files,default is open mode.
    dialog.setNameFilter(".csv");
    QString filename = dialog.getSaveFileName(this,"Save parameters file...",QDir::currentPath(),".txt");

    std::ofstream output;
    output.open(filename.toStdString().c_str());

    int nStair = ui->tabStaircase->count();

    output << "Mu " << ui->doubleSpinBoxMean->value() << endl;
    output << "Sigma: " << ui->doubleSpinBoxStd->value() << endl;
    output << "LapseRate: " << ui->doubleSpinBoxLapseRate->value() << endl;
    output << "GuessRate: " << ui->doubleSpinBoxGuessRate->value() << endl;
    output << "sStairNumber: " <<   nStair << endl;
    output << "sStairStartStates: "  ;

    for (int i=0; i< nStair; i++ )
    {
        output << ui->tabStaircase->getStaircaseParamsSetter(i)->getStartState() ;
        if ( i < nStair-1 )
            output << ",";
    }
    output << endl;

    output << "sStairAscending: " ;
    for (int i=0; i< nStair; i++ )
    {
        output << ui->tabStaircase->getStaircaseParamsSetter(i)->getAscendingStaircase() ;
        if ( i < nStair-1 )
            output << ",";
    }
    output << endl;

    output << "sStairCorrectAnswers: " ;
    for (int i=0; i< nStair; i++ )
    {
        output << ui->tabStaircase->getStaircaseParamsSetter(i)->getAscendingStaircase();
        if ( i < nStair-1 )
            output << ",";
    }

    output << endl;

    output << "sStairMaxInversions: " ;
    for (int i=0; i< nStair; i++ )
    {
        output << ui->tabStaircase->getStaircaseParamsSetter(i)->getMaxReversals();
        if ( i < nStair-1 )
            output << ",";
    }

    output << endl;

    output << "sStairMaxTrials: " ;
    for (int i=0; i< nStair; i++ )
    {
        output << ui->tabStaircase->getStaircaseParamsSetter(i)->getMaxTrials() ;
        if ( i < nStair-1 )
            output << ",";
    }

    output << endl;
    output << "sStairPositiveStep: " ;
    for (int i=0; i< nStair; i++ )
    {
        output << ui->tabStaircase->getStaircaseParamsSetter(i)->getPositiveStepsString() ;
        if ( i < nStair-1 )
            output << ",";
    }

    output << endl;
    output << "sStairNegativeStep: " ;
    for (int i=0; i< 1; i++ )
    {
        output << ui->tabStaircase->getStaircaseParamsSetter(i)->getNegativeStep() ;
    }
    output << endl;

    output << "sStairClampLower: " ;
    for (int i=0; i< nStair; i++ )
    {
        output << ui->tabStaircase->getStaircaseParamsSetter(i)->getLowerClamp() ;
        if ( i < nStair-1 )
            output << ",";
    }
 output << endl;
    output << "sStairClampUpper: " ;
    for (int i=0; i< nStair; i++ )
    {
        output << ui->tabStaircase->getStaircaseParamsSetter(i)->getUpperClamp() ;
        if ( i < nStair-1 )
            output << ",";
    }
 output << endl;
    output << "sStairMaxClampHits: " ;
    for (int i=0; i< nStair; i++ )
    {
        output << ui->tabStaircase->getStaircaseParamsSetter(i)->getMaxClampHits() ;
        if ( i < nStair-1 )
            output << ",";
    }
     output << endl;
}
