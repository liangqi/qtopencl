TEMPLATE = subdirs
CONFIG += ordered
config_opencl {
    SUBDIRS += opencl

    config_openclgl {
        SUBDIRS += openclgl
    }
}
