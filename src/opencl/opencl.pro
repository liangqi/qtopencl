TEMPLATE = lib
TARGET = QtOpenCL
QT += opengl
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
    qclcontextgl.h \
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
    qcltexture2d.h \
    qclworksize.h

SOURCES += \
    qclbuffer.cpp \
    qclcommandqueue.cpp \
    qclcontext.cpp \
    qclcontextgl.cpp \
    qcldevice.cpp \
    qclevent.cpp \
    qclimage.cpp \
    qclimageformat.cpp \
    qclkernel.cpp \
    qclmemoryobject.cpp \
    qclplatform.cpp \
    qclprogram.cpp \
    qclsampler.cpp \
    qcltexture2d.cpp \
    qclworksize.cpp

PRIVATE_HEADERS += \
    qcl_gl_p.h

HEADERS += $$PRIVATE_HEADERS
DEFINES += QT_BUILD_OPENCL_LIB
