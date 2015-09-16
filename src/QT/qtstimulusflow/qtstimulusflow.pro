VPATH += ../shared
INCLUDEPATH += ../shared ../../eigen/ ../../experiment ../../core ../../glviz/
LIBS += -lglut

HEADERS       =  \
    StimulusWidget.h \
    ../../experiment/Stimulus.h \
    ../../experiment/BoxNoiseStimulus.h \
    ../../glviz/StimulusDrawer.h \
    ../../experiment/PointwiseStimulus.h \
    ../../core/Util.h \
    ../../core/Point3D.h \
    ../../core/Mathcommon.h \
    ../../glviz/VRCamera.h \
    ../../glviz/GLUtils.h \
    ../../core/Screen.h \
    MainWindow.h \
    ../../glviz/Grid.h \
    QCustomPlot.h
SOURCES       =  main.cpp \
    StimulusWidget.cpp \
    ../../experiment/BoxNoiseStimulus.cpp \
    ../../glviz/StimulusDrawer.cpp \
    ../../experiment/PointwiseStimulus.cpp \
    ../../core/Point3D.cpp \
    ../../core/Mathcommon.cpp \
    ../../glviz/VRCamera.cpp \
    ../../glviz/GLUtils.cpp \
    ../../core/Screen.cpp \
    MainWindow.cpp \
    ../../glviz/Grid.cpp \
    QCustomPlot.cpp
QT           += opengl

# install
target.path = $$[QT_INSTALL_EXAMPLES]/opengl/hellogl
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS hellogl.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/opengl/hellogl
INSTALLS += target sources

FORMS += \
    MainWindow.ui
