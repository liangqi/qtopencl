TARGET     = QtOpenCLGL
QT         += core-private gui-private concurrent opengl opencl

win32-msvc*|win32-icc:QMAKE_LFLAGS += /BASE:0x66000000
solaris-cc*:QMAKE_CXXFLAGS_RELEASE -= -O2

#QMAKE_DOCS = $$PWD/doc/qtopenclgl.qdocconf
load(qt_module)

win32 {
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

macx: {
    LIBS += -framework OpenCL
}

no_cl_gl {
    DEFINES += QT_NO_CL_OPENGL
}

INCLUDEPATH += $$PWD/../opencl

HEADERS += \
    qclcontextgl.h

SOURCES += \
    qclcontextgl.cpp

PRIVATE_HEADERS += \
    qcl_gl_p.h

HEADERS += $$PRIVATE_HEADERS
DEFINES += QT_BUILD_CLGL_LIB
