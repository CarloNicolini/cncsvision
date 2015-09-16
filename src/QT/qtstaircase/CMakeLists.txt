set(QTSTAIRCASE_HDR mainwindow.h  QCustomPlot.h  StaircaseParamSetter.h  TabStaircase.h)
set(QTSTAIRCASE_SRC main.cpp  mainwindow.cpp  QCustomPlot.cpp  StaircaseParamSetter.cpp  TabStaircase.cpp )
set(QTSTAIRCASE_FORMS form2.ui  form.ui  mainwindow.ui  StaircaseParamSetter.ui)
set(QTSTAIRCASE_RESOURCES "resources.qrc")
link_libraries(${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} )
include_directories(${QT_INCLUDE_PATH} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/src/core ${CMAKE_SOURCE_DIR}/src/experiment ${CMAKE_SOURCE_DIR}/src/eigen QCustomPlot )
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDES})
QT4_ADD_RESOURCES( QTSTAIRCASE_RESOURCES_SOURCES ${QTSTAIRCASE_RESOURCES} )
QT4_WRAP_UI( QTSTAIRCASE_FORMS_HEADERS ${QTSTAIRCASE_FORMS} )
QT4_WRAP_CPP( QTSTAIRCASE_HEADERS_MOC ${QTSTAIRCASE_HDR} )

add_executable(QtStaircase WIN32 MACOSX_BUNDLE ${QTSTAIRCASE_SRC} ${QTSTAIRCASE_HDR} ${QTSTAIRCASE_HEADERS_MOC}
	${QTSTAIRCASE_RESOURCES_SOURCES} ${QTSTAIRCASE_FORMS_HEADERS} ${QTSTAIRCASE_RCS})
target_link_libraries(QtStaircase ${QT_LIBRARIES} Core Experiment)
