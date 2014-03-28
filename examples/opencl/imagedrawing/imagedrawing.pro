TEMPLATE = app
TARGET = imagedrawing
DEPENDPATH += .
INCLUDEPATH += .
QT += opencl widgets
# Input
SOURCES += main.cpp \
           imagewidget.cpp
HEADERS += imagewidget.h
RESOURCES += imagedrawing.qrc
