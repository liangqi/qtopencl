TEMPLATE = app
TARGET = bezierpatch
DEPENDPATH += .
INCLUDEPATH += .
QT += opengl opencl openclgl concurrent widgets

VPATH += ../mandelbrot
INCLUDEPATH += ../mandelbrot

# Input
SOURCES += main.cpp \
           beziermainwindow.cpp \
           bezierwidget.cpp \
           qglcamera.cpp \
           framerate.cpp
HEADERS += bezierwidget.h \
           beziermainwindow.h \
           qglcamera.h \
           framerate.h
RESOURCES += bezierpatch.qrc
FORMS += beziermainwindow.ui
