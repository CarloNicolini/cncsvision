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

#ifndef STAIRCASEPARAMSETTER_H
#define STAIRCASEPARAMSETTER_H

#include <QWidget>
#include <string>

namespace Ui {
    class StaircaseParamSetter;
}

class StaircaseParamSetter : public QWidget
{
    Q_OBJECT

public:
    explicit StaircaseParamSetter(QWidget *parent = 0);
    ~StaircaseParamSetter();

    bool getAscendingStaircase();
    double getStartState();
    int getCorrectAnswers();
    double getLowerClamp();
    double getUpperClamp();
    int getMaxReversals();
    int getMaxTrials();
    int getMaxClampHits();
    bool isClampActive();
    std::vector<double> getPositiveSteps();
    std::string getPositiveStepsString();
    double getNegativeStep();
    bool isStringIDset();
    QString getStaircaseName();

    void setAscendingStaircase(bool);
    void setStartState(double);
    void setCorrectAnswers(double);
    void  setLowerClamp(double);
    void  setUpperClamp(double);
    void  setMaxReversals(int);
    void  setMaxTrials(int);
    void  setMaxClampHits(int);
    void  setNegativeStep(double);
    void setClampActive(bool val);
    void setPositiveSteps(const std::string &str);

    void pushState(double s);
//    const QVector<double> & getStates();


private:
    int id;
    QVector<double> states;
    Ui::StaircaseParamSetter *ui;

private slots:

public:
    static int StaircaseTabNumber;
};

#endif // STAIRCASEPARAMSETTER_H
