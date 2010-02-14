TEMPLATE = lib
TARGET = QtOpenCL
CONFIG += dll \
    warn_on
win32 { 
    DESTDIR = ../../bin
    !static:DEFINES += QT_MAKEDLL
}
else:DESTDIR = ../../lib

macx {
    LIBS += -framework OpenCL
}
HEADERS += \
    qclbuffer.h \
    qclcommandqueue.h \
    qclcontext.h \
    qcldevice.h \
    qclevent.h \
    qclglobal.h \
    qclimage.h \
    qclimageformat.h \
    qclkernel.h \
    qclmemoryobject.h \
    qclplatform.h \
    qclprogram.h \
    qclsampler.h \
    qclworksize.h

SOURCES += \
    qclbuffer.cpp \
    qclcommandqueue.cpp \
    qclcontext.cpp \
    qcldevice.cpp \
    qclevent.cpp \
    qclimage.cpp \
    qclimageformat.cpp \
    qclkernel.cpp \
    qclmemoryobject.cpp \
    qclplatform.cpp \
    qclprogram.cpp \
    qclsampler.cpp \
    qclworksize.cpp

HEADERS += $$PRIVATE_HEADERS
DEFINES += QT_BUILD_OPENCL_LIB
