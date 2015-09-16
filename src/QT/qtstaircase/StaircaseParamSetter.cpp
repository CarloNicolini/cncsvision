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


#include "StaircaseParamSetter.h"
#include "ui_StaircaseParamSetter.h"
#include <iostream>
#include <QtGui>
#include "Util.h"

using namespace std;
int StaircaseParamSetter::StaircaseTabNumber=-1;

StaircaseParamSetter::StaircaseParamSetter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StaircaseParamSetter)
{
    ui->setupUi(this);

    ui->ascendingRadio->setChecked(true);
    ui->textEdit->setText("0.2");

    ui->doubleSpinBoxUpperClamp->setDisabled(true);
    ui->doubleSpinBoxLowerClamp->setDisabled(true);
    ui->spinBoxNClamp->setDisabled(true);
    ui->lineEditStringID->setDisabled(true);

    connect(ui->checkBoxClampActive,SIGNAL(clicked(bool)),ui->spinBoxNClamp,SLOT(setEnabled(bool)));
    connect(ui->checkBoxClampActive,SIGNAL(clicked(bool)),ui->doubleSpinBoxLowerClamp,SLOT(setEnabled(bool)));
    connect(ui->checkBoxClampActive,SIGNAL(clicked(bool)),ui->doubleSpinBoxUpperClamp,SLOT(setEnabled(bool)));

    connect(ui->checkBoxStringID,SIGNAL(clicked(bool)),ui->lineEditStringID,SLOT(setEnabled(bool)));
    StaircaseTabNumber++;
    id=StaircaseTabNumber;
    //cerr << "StaircaseParamSetter constructed id= " << id << endl;
}

StaircaseParamSetter::~StaircaseParamSetter()
{
    StaircaseTabNumber--;
    //cerr << "StaircaseParamSetter destructed" << endl;
   // delete ui;
}

bool StaircaseParamSetter::getAscendingStaircase()
{
    return ui->ascendingRadio->isChecked();
}

double StaircaseParamSetter::getStartState()
{
    return ui->doubleSpinBoxStartState->value();
}


double StaircaseParamSetter::getLowerClamp()
{
    return ui->doubleSpinBoxLowerClamp->value();
}

double StaircaseParamSetter::getUpperClamp()
{
    return ui->doubleSpinBoxUpperClamp->value();
}
bool StaircaseParamSetter::isClampActive()
{
    return ui->checkBoxClampActive->isChecked();
}

void StaircaseParamSetter::setClampActive(bool val)
{
    ui->checkBoxClampActive->setChecked(val);
    ui->doubleSpinBoxLowerClamp->setEnabled(true);
    ui->doubleSpinBoxUpperClamp->setEnabled(true);
    ui->spinBoxNClamp->setEnabled(true);

}

int StaircaseParamSetter::getMaxReversals()
{
     return ui->spinBoxNInversions->value();
}
int StaircaseParamSetter::getMaxTrials()
{
     return ui->spinBoxNTrials->value();
}
int StaircaseParamSetter::getMaxClampHits()
{
     return ui->spinBoxNClamp->value();
}
int StaircaseParamSetter::getCorrectAnswers()
{
    return ui->spinBoxCorrectAnswers->value();
}

std::string StaircaseParamSetter::getPositiveStepsString()
{
    return ui->textEdit->toPlainText().toStdString();
}

vector<double> StaircaseParamSetter::getPositiveSteps()
{
     std::string text = ui->textEdit->toPlainText().replace(" ","").toStdString();

    return util::str2num<double>(text,",");
}

double StaircaseParamSetter::getNegativeStep()
{
    return ui->doubleSpinBoxNegativeStep->value();
}

void StaircaseParamSetter::setAscendingStaircase(bool val)
{
        ui->ascendingRadio->setChecked(val);
        ui->descendingRadio->setChecked(!val);
}

void StaircaseParamSetter::setStartState(double val)
{
    ui->doubleSpinBoxStartState->setValue(val);
}
void StaircaseParamSetter::setCorrectAnswers(double val)
{
    ui->spinBoxCorrectAnswers->setValue(val);
}
void StaircaseParamSetter::setLowerClamp(double val)
{
    ui->doubleSpinBoxLowerClamp->setValue(val);
}
void StaircaseParamSetter::setUpperClamp(double val)
{
    ui->doubleSpinBoxUpperClamp->setValue(val);
}
void StaircaseParamSetter::setMaxReversals(int val)
{
    ui->spinBoxNInversions->setValue(val);
}
void StaircaseParamSetter::setMaxTrials(int val)
{
    ui->spinBoxNTrials->setValue(val);
}
void StaircaseParamSetter::setMaxClampHits(int val)
{
    ui->spinBoxNClamp->setValue(val);
}

void StaircaseParamSetter::setNegativeStep(double val)
{
    ui->doubleSpinBoxNegativeStep->setValue(val);
}

void StaircaseParamSetter::setPositiveSteps(const std::string &str)
{
    ui->textEdit->setText(QString(str.c_str()));
}

bool StaircaseParamSetter::isStringIDset()
{
    return ui->checkBoxStringID->isChecked();
}

QString StaircaseParamSetter::getStaircaseName()
{
    if ( isStringIDset() )
    return ui->lineEditStringID->text();
    else
        return QString::number(this->id);
}

void StaircaseParamSetter::pushState(double s)
{
    states.push_back(s);
}
