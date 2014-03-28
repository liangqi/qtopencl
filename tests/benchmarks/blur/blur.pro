TEMPLATE = app
QT += testlib opencl widgets
CONFIG += unittest \
    warn_on
SOURCES += tst_blur.cpp \
    pixmapfilterwidget.cpp \
    graphicseffectview.cpp \
    clwidget.cpp
RESOURCES += blur.qrc \
    images/images.qrc
OTHER_FILES += blur.cl
HEADERS += pixmapfilterwidget.h \
    graphicseffectview.h \
    clwidget.h
