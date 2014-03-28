SOURCES += openclgl.cpp
CONFIG += opengl

macx: {
    LIBS += -framework OpenCL
}
