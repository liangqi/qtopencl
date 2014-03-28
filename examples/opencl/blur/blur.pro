TEMPLATE = app
TARGET = blur
DEPENDPATH += .
INCLUDEPATH += .
QT += opencl widgets
# Input
SOURCES += main.cpp \
           blurwidget.cpp
HEADERS += blurwidget.h \
           blurtable.h
RESOURCES += blur.qrc
