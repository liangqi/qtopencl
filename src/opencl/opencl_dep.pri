INCLUDEPATH += $$PWD
macx: {
    LIBS += -lQtOpenCL -framework OpenCL
}
else: LIBS += -lQtOpenCL -lOpenCL
QT += opengl
