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

#ifndef TABSTAIRCASE_H
#define TABSTAIRCASE_H

#include <QTabWidget>
#include <deque>
#include "ParStaircase.h"
#include "StaircaseParamSetter.h"

class TabStaircase : public QTabWidget
{
    Q_OBJECT
public:
    explicit TabStaircase(QWidget *parent = 0);
    ~TabStaircase();
    StaircaseParamSetter* getStaircaseParamsSetter(int i);
    int getNumberOfTabs();
    void reset();

signals:

public slots:
    void setNumberOfTabs(int n);

private:
    std::deque<StaircaseParamSetter *> stairSettings;
};

#endif // TABSTAIRCASE_H
