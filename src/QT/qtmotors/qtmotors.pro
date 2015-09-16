#-------------------------------------------------
#
# Project created by QtCreator 2012-02-29T09:53:22
#
#-------------------------------------------------

QT       += core gui

TARGET = qtmotors
TEMPLATE = app


win32:LIBS+= -L"C:/Progra~2/boost/boost_1_47/lib"
win32:INCLUDEPATH+= C:/Progra~2/boost/boost_1_47

#LIBS += -lboost_filesystem -lboost_thread

INCLUDEPATH += . ../../libs/eigen/ ../../libs/communications/ ../../libs/geometry/

SOURCES += main.cpp\
        MainWindow.cpp \
    ../../libs/communications/SerialStream.cpp \
    ../../libs/communications/SerialMotor.cpp \
    ../../libs/communications/AsyncSerial.cpp \
    ../../libs/geometry/Mathcommon.cpp \
    InfoDialog.cpp

HEADERS  += MainWindow.h \
    ../../libs/communications/SerialStream.h \
    ../../libs/communications/SerialMotor.h \
    ../../libs/communications/BufferedAsyncSerial.h \
    ../../libs/communications/AsyncSerial.h \
    ../../libs/geometry/Util.h \
    ../../libs/geometry/Mathcommon.h \
    InfoDialog.h

FORMS    += MainWindow.ui \
    InfoDialog.ui

win32:RC_FILE = iconwin32.rc

RESOURCES += \
    resources.qrc
