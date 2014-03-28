TEMPLATE = app
TARGET = pathdrawing
DEPENDPATH += .
INCLUDEPATH += .
QT += opencl widgets
# Input
SOURCES += main.cpp \
           pathwidget.cpp
HEADERS += pathwidget.h
RESOURCES += pathdrawing.qrc
