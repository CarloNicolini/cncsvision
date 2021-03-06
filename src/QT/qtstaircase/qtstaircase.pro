######################################################################
# Automatically generated by qmake (2.01a) Thu Feb 23 12:24:42 2012
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += . ../../libs/experiment ../../libs/eigen/ ../../libs/geometry/

win32:LIBS+= -L"C:/Progra~2/boost/boost_1_46/lib"
win32:INCLUDEPATH+= C:/Progra~2/boost/boost_1_46

# Input
HEADERS += mainwindow.h QCustomPlot.h StaircaseParamSetter.h TabStaircase.h \
    ../../libs/experiment/Staircase.h \
    ../../libs/experiment/ParStaircase.h \
    ../../libs/experiment/ParametersLoader.h \
    ../../libs/geometry/Util.h \
    ../../libs/geometry/Timer.h \
    ../../libs/geometry/Mathcommon.h
FORMS += mainwindow.ui StaircaseParamSetter.ui
SOURCES += main.cpp \
           mainwindow.cpp \
           QCustomPlot.cpp \
           StaircaseParamSetter.cpp \
           TabStaircase.cpp \
    ../../libs/experiment/Staircase.cpp \
    ../../libs/experiment/ParStaircase.cpp \
    ../../libs/experiment/ParametersLoader.cpp \
    ../../libs/geometry/Timer.cpp \
    ../../libs/geometry/Mathcommon.cpp
