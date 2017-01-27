#-------------------------------------------------
#
# Project created by QtCreator 2016-04-20T16:21:30
#
#-------------------------------------------------

QT       += core gui

QT += widgets
QT += printsupport

QMAKE_CXXFLAGS += /openmp  #visual studio
LIBS += /openmp  #visual studio

TARGET = KPGraph
TEMPLATE = app


SOURCES += main.cpp\
    Visualizations/GGraph/ggraphparser.cpp \
    Visualizations/GGraph/Base/ggraph.cpp \
    Visualizations/GGraph/Base/ggraphdrawing.cpp

HEADERS  += \
    Visualizations/GGraph/ggraphparser.h \
    Visualizations/GGraph/Base/ggraph.h \
    Visualizations/GGraph/Base/ggraphdrawing.h

FORMS    +=

INCLUDEPATH += Visualizations

RESOURCES += \
    resources.qrc
