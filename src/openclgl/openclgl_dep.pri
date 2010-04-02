INCLUDEPATH += $$PWD $$PWD/../opencl
macx {
    LIBS += -lQtOpenCLGL -lQtOpenCL -framework OpenCL
} else {
    LIBS += -lQtOpenCLGL -lQtOpenCL
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
QT += opengl
no_cl_gl {
    DEFINES += QT_NO_CL_OPENGL
}
