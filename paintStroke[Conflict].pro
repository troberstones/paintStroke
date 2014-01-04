#-------------------------------------------------
#
# Project created by QtCreator 2013-12-12T20:43:28
#
#-------------------------------------------------

QT       += core gui opengl widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = paintStroke
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    glwidget.cpp \
    strokedb.cpp \
    testglwidget.cpp

HEADERS  += mainwindow.h \
    glwidget.h \
    strokedb.h \
    testglwidget.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    basic.frag \
    basic.vert
