load(qttest_p4.prf)
TEMPLATE=app
QT += testlib concurrent opencl
CONFIG += unittest warn_on

SOURCES += tst_qcl.cpp
RESOURCES += tst_qcl.qrc
