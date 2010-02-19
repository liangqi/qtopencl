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
} else {
    !isEmpty(QMAKE_INCDIR_OPENCL) {
        QMAKE_CXXFLAGS += -I$$QMAKE_INCDIR_OPENCL
    }
    !isEmpty(QMAKE_LIBDIR_OPENCL) {
        LIBS += -L$$QMAKE_LIBDIR_OPENCL
    }
    !isEmpty(QMAKE_LIBS_OPENCL) {
        LIBS += $$QMAKE_LIBS_OPENCL
    } else {
        LIBS += -lOpenCL
    }
}

no_cl_gl {
    DEFINES += QT_NO_CL_OPENGL
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
    qclnamespace.h \
    qclplatform.h \
    qclprogram.h \
    qclsampler.h \
    qcltexture2d.h \
    qclvector.h \
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
    qclvector.cpp \
    qclworksize.cpp

PRIVATE_HEADERS += \
    qcl_gl_p.h

HEADERS += $$PRIVATE_HEADERS
DEFINES += QT_BUILD_CL_LIB
