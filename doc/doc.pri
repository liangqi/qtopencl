
# Determine which qdoc3 binary we should be using.  We try to use the
# one from the Qt we built against.  Otherwise use "qdoc3" on the PATH.
win32:!win32-g++ {
    unixstyle = false
} else :win32-g++:isEmpty(QMAKE_SH) {
    unixstyle = false
} else {
    unixstyle = true
}
$$unixstyle {
    exists($$[QT_INSTALL_BINS]/qdoc3) {
        QDOC = $$[QT_INSTALL_BINS]/qdoc3
    } else {
        QDOC = qdoc3
    }
} else {
    exists($$[QT_INSTALL_BINS]\\qdoc3.exe) {
        QDOC = $$[QT_INSTALL_BINS]\\qdoc3
    } else {
        QDOC = qdoc3
    }
}

TOPSRC          = $$PWD/..
QDOCCONF_FILE   = $$PWD/src/qtopencl.qdocconf
DESTDIR         = $$OUT_PWD/doc/html

docs.commands = (TOPSRC=$$TOPSRC DESTDIR=$$DESTDIR $$QDOC $$QDOCCONF_FILE)

QMAKE_EXTRA_TARGETS += docs
