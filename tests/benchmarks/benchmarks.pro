TEMPLATE = subdirs
SUBDIRS += mandelbrot
contains(QT_CONFIG, private_tests): SUBDIRS += blur
