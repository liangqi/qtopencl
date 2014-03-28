SOURCES += opencl.cpp
CONFIG -= qt

macx: {
    LIBS += -framework OpenCL
}
