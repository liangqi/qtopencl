SOURCES += opencl11.cpp
CONFIG -= qt

macx: {
    LIBS += -framework OpenCL
}
