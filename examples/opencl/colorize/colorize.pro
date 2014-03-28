TEMPLATE = app
TARGET = colorize
DEPENDPATH += .
INCLUDEPATH += .
QT += opencl widgets
# Input
SOURCES += main.cpp \
           colorizewidget.cpp
HEADERS += colorizewidget.h
RESOURCES += colorize.qrc
