/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenCL module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qclcontext.h"
#include <QtCore/qdebug.h>
#include <QtCore/qvarlengtharray.h>
#include <QtCore/qfile.h>

QT_BEGIN_NAMESPACE

/*!
    \class QCLContext
    \brief The QCLContext class represents an OpenCL context.
    \since 4.7
    \ingroup opencl

    \sa QCLContextGL
*/

class QCLContextPrivate
{
public:
    QCLContextPrivate()
        : id(0)
        , isCreated(false)
        , lastError(CL_SUCCESS)
    {
    }
    ~QCLContextPrivate()
    {
        // Release the command queues for the context.
        commandQueue = QCLCommandQueue();
        defaultCommandQueue = QCLCommandQueue();

        // Release the context.
        if (isCreated)
            clReleaseContext(id);
    }

    cl_context id;
    bool isCreated;
    QCLPlatform platform;
    QCLCommandQueue commandQueue;
    QCLCommandQueue defaultCommandQueue;
    cl_int lastError;
};

/*!
    Constructs a new OpenCL context object.  This constructor is
    typically followed by calls to setPlatform() and create().
*/
QCLContext::QCLContext()
    : d_ptr(new QCLContextPrivate())
{
}

/*!
    Destroys this OpenCL context object.  If the underlying
    context id() has been created, then it will be released.
*/
QCLContext::~QCLContext()
{
}

/*!
    Returns true if the underlying OpenCL context id() has been
    created; false otherwise.

    \sa create(), setId()
*/
bool QCLContext::isCreated() const
{
    Q_D(const QCLContext);
    return d->isCreated;
}

/*!
    Returns the platform that the context will be created within
    when create() is called.  The default is a null QCLPlatform,
    which indicates that no specific platform is required.

    After the context has been created, the actual platform in use
    by the context can be retrieved by calling QCLDevice::platform()
    on defaultDevice().

    \sa setPlatform(), create()
*/
QCLPlatform QCLContext::platform() const
{
    Q_D(const QCLContext);
    return d->platform;
}

/*!
    Sets the \a platform to be used when the context is created
    by create().

    \sa platform(), create()
*/
void QCLContext::setPlatform(const QCLPlatform &platform)
{
    Q_D(QCLContext);
    d->platform = platform;
}

extern "C" {

static void qt_cl_context_notify(const char *errinfo,
                                 const void *private_info,
                                 size_t cb,
                                 void *user_data)
{
    Q_UNUSED(private_info);
    Q_UNUSED(cb);
    Q_UNUSED(user_data);
    qWarning() << "OpenCL context notification: " << errinfo;
}

};

/*!
    Creates a new OpenCL context that matches platform() and
    \a type.  Does nothing if the context has already been created.

    Returns true if the context was created; false otherwise.
    On error, the status can be retrieved by calling lastError().

    \sa isCreated(), setId(), release()
*/
bool QCLContext::create(QCLDevice::DeviceTypes type)
{
    Q_D(QCLContext);
    if (d->isCreated)
        return true;
    if (!d->platform.isNull()) {
        cl_context_properties props[] = {
            CL_CONTEXT_PLATFORM,
            cl_context_properties(d->platform.id()),
            0
        };
        d->id = clCreateContextFromType
            (props, cl_device_type(type),
             qt_cl_context_notify, 0, &(d->lastError));
    } else {
        d->id = clCreateContextFromType
            (0, cl_device_type(type),
             qt_cl_context_notify, 0, &(d->lastError));
    }
    d->isCreated = (d->id != 0);
    if (!d->isCreated) {
        qWarning() << "QCLContext::create(type:" << int(type) << "):"
                   << errorName(d->lastError);
    }
    return d->isCreated;
}

/*!
    Creates a new OpenCL context that matches platform() and
    \a devices.  Does nothing if the context has already been created.

    Returns true if the context was created; false otherwise.
    On error, the status can be retrieved by calling lastError().

    \sa isCreated(), setId(), release()
*/
bool QCLContext::create(const QList<QCLDevice> &devices)
{
    Q_D(QCLContext);
    if (d->isCreated)
        return true;
    QVector<cl_device_id> devs;
    foreach (QCLDevice dev, devices)
        devs.append(dev.id());
    if (!d->platform.isNull()) {
        cl_context_properties props[] = {
            CL_CONTEXT_PLATFORM,
            intptr_t(d->platform.id()),
            0
        };
        d->id = clCreateContext
            (props, devs.size(), devs.constData(),
             qt_cl_context_notify, 0, &(d->lastError));
    } else {
        d->id = clCreateContext
            (0, devs.size(), devs.constData(),
             qt_cl_context_notify, 0, &(d->lastError));
    }
    d->isCreated = (d->id != 0);
    if (!d->isCreated)
        qWarning() << "QCLContext::create:" << errorName(d->lastError);
    return d->isCreated;
}

/*!
    Releases this context; destroying it if the reference count is zero.
    Does nothing if the context has not been created or is already released.

    \sa create()
*/
void QCLContext::release()
{
    Q_D(QCLContext);
    if (d->isCreated) {
        d->commandQueue = QCLCommandQueue();
        d->defaultCommandQueue = QCLCommandQueue();
        clReleaseContext(d->id);
        d->id = 0;
        d->isCreated = false;
    }
}

/*!
    Returns the native OpenCL context identifier associated
    with this object.

    \sa setId()
*/
cl_context QCLContext::id() const
{
    Q_D(const QCLContext);
    return d->id;
}

/*!
    Sets the native OpenCL context identifier associated with this
    object to \a id.

    This function will call \c{clRetainContext()} to increase the
    reference count on \a id.  If the identifier was previously set
    to something else, then \c{clReleaseContext()} will be called
    on the previous value.

    \sa id(), create()
*/
void QCLContext::setId(cl_context id)
{
    Q_D(QCLContext);
    if (d->id == id || !id)
        return;
    release();
    clRetainContext(id);
    d->id = id;
    d->isCreated = true;
}

/*!
    Returns the list of devices that are in use by this context.
    If the context has not been created, returns an empty list.

    \sa defaultDevice()
*/
QList<QCLDevice> QCLContext::devices() const
{
    Q_D(const QCLContext);
    QList<QCLDevice> devs;
    if (d->isCreated) {
        size_t size = 0;
        if (clGetContextInfo(d->id, CL_CONTEXT_DEVICES, 0, 0, &size)
                == CL_SUCCESS && size > 0) {
            QVarLengthArray<cl_device_id> buf(size);
            if (clGetContextInfo(d->id, CL_CONTEXT_DEVICES,
                                 size, buf.data(), 0) == CL_SUCCESS) {
                for (size_t index = 0; index < size; ++index)
                    devs.append(QCLDevice(buf[index]));
            }
        }
    }
    return devs;
}

/*!
    Returns the default device in use by this context, which is the
    first element of the devices() list; or a null QCLDevice if the
    context has not been created yet.

    \sa devices()
*/
QCLDevice QCLContext::defaultDevice() const
{
    Q_D(const QCLContext);
    if (d->isCreated) {
        size_t size = 0;
        if (clGetContextInfo(d->id, CL_CONTEXT_DEVICES, 0, 0, &size)
                == CL_SUCCESS && size > 0) {
            QVarLengthArray<cl_device_id> buf(size);
            if (clGetContextInfo(d->id, CL_CONTEXT_DEVICES,
                                 size, buf.data(), 0) == CL_SUCCESS) {
                return QCLDevice(buf[0]);
            }
        }
    }
    return QCLDevice();
}

/*!
    Returns the last OpenCL error that occurred while executing an
    operation on this context or any of the objects created by
    the context.  Returns CL_SUCCESS if the last operation succeeded.

    \sa setLastError(), errorName()
*/
cl_int QCLContext::lastError() const
{
    Q_D(const QCLContext);
    return d->lastError;
}

/*!
    Sets the last error code to \a error.

    \sa lastError(), errorName()
*/
void QCLContext::setLastError(cl_int error)
{
    Q_D(QCLContext);
    d->lastError = error;
}

/*!
    Returns the name of the supplied OpenCL error \a code.  For example,
    \c{CL_SUCCESS}, \c{CL_INVALID_CONTEXT}, etc.

    \sa lastError()
*/
QString QCLContext::errorName(cl_int code)
{
    switch (code) {
#ifdef CL_SUCCESS
    case CL_SUCCESS: return QLatin1String("CL_SUCCESS");
#endif
#ifdef CL_DEVICE_NOT_FOUND
    case CL_DEVICE_NOT_FOUND: return QLatin1String("CL_DEVICE_NOT_FOUND");
#endif
#ifdef CL_DEVICE_NOT_AVAILABLE
    case CL_DEVICE_NOT_AVAILABLE: return QLatin1String("CL_DEVICE_NOT_AVAILABLE");
#endif
#ifdef CL_COMPILER_NOT_AVAILABLE
    case CL_COMPILER_NOT_AVAILABLE: return QLatin1String("CL_COMPILER_NOT_AVAILABLE");
#endif
#ifdef CL_MEM_OBJECT_ALLOCATION_FAILURE
    case CL_MEM_OBJECT_ALLOCATION_FAILURE: return QLatin1String("CL_MEM_OBJECT_ALLOCATION_FAILURE");
#endif
#ifdef CL_OUT_OF_RESOURCES
    case CL_OUT_OF_RESOURCES: return QLatin1String("CL_OUT_OF_RESOURCES");
#endif
#ifdef CL_OUT_OF_HOST_MEMORY
    case CL_OUT_OF_HOST_MEMORY: return QLatin1String("CL_OUT_OF_HOST_MEMORY");
#endif
#ifdef CL_PROFILING_INFO_NOT_AVAILABLE
    case CL_PROFILING_INFO_NOT_AVAILABLE: return QLatin1String("CL_PROFILING_INFO_NOT_AVAILABLE");
#endif
#ifdef CL_MEM_COPY_OVERLAP
    case CL_MEM_COPY_OVERLAP: return QLatin1String("CL_MEM_COPY_OVERLAP");
#endif
#ifdef CL_IMAGE_FORMAT_MISMATCH
    case CL_IMAGE_FORMAT_MISMATCH: return QLatin1String("CL_IMAGE_FORMAT_MISMATCH");
#endif
#ifdef CL_IMAGE_FORMAT_NOT_SUPPORTED
    case CL_IMAGE_FORMAT_NOT_SUPPORTED: return QLatin1String("CL_IMAGE_FORMAT_NOT_SUPPORTED");
#endif
#ifdef CL_BUILD_PROGRAM_FAILURE
    case CL_BUILD_PROGRAM_FAILURE: return QLatin1String("CL_BUILD_PROGRAM_FAILURE");
#endif
#ifdef CL_MAP_FAILURE
    case CL_MAP_FAILURE: return QLatin1String("CL_MAP_FAILURE");
#endif
#ifdef CL_INVALID_VALUE
    case CL_INVALID_VALUE: return QLatin1String("CL_INVALID_VALUE");
#endif
#ifdef CL_INVALID_DEVICE_TYPE
    case CL_INVALID_DEVICE_TYPE: return QLatin1String("CL_INVALID_DEVICE_TYPE");
#endif
#ifdef CL_INVALID_PLATFORM
    case CL_INVALID_PLATFORM: return QLatin1String("CL_INVALID_PLATFORM");
#endif
#ifdef CL_INVALID_DEVICE
    case CL_INVALID_DEVICE: return QLatin1String("CL_INVALID_DEVICE");
#endif
#ifdef CL_INVALID_CONTEXT
    case CL_INVALID_CONTEXT: return QLatin1String("CL_INVALID_CONTEXT");
#endif
#ifdef CL_INVALID_QUEUE_PROPERTIES
    case CL_INVALID_QUEUE_PROPERTIES: return QLatin1String("CL_INVALID_QUEUE_PROPERTIES");
#endif
#ifdef CL_INVALID_COMMAND_QUEUE
    case CL_INVALID_COMMAND_QUEUE: return QLatin1String("CL_INVALID_COMMAND_QUEUE");
#endif
#ifdef CL_INVALID_HOST_PTR
    case CL_INVALID_HOST_PTR: return QLatin1String("CL_INVALID_HOST_PTR");
#endif
#ifdef CL_INVALID_MEM_OBJECT
    case CL_INVALID_MEM_OBJECT: return QLatin1String("CL_INVALID_MEM_OBJECT");
#endif
#ifdef CL_INVALID_IMAGE_FORMAT_DESCRIPTOR
    case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR: return QLatin1String("CL_INVALID_IMAGE_FORMAT_DESCRIPTOR");
#endif
#ifdef CL_INVALID_IMAGE_SIZE
    case CL_INVALID_IMAGE_SIZE: return QLatin1String("CL_INVALID_IMAGE_SIZE");
#endif
#ifdef CL_INVALID_SAMPLER
    case CL_INVALID_SAMPLER: return QLatin1String("CL_INVALID_SAMPLER");
#endif
#ifdef CL_INVALID_BINARY
    case CL_INVALID_BINARY: return QLatin1String("CL_INVALID_BINARY");
#endif
#ifdef CL_INVALID_BUILD_OPTIONS
    case CL_INVALID_BUILD_OPTIONS: return QLatin1String("CL_INVALID_BUILD_OPTIONS");
#endif
#ifdef CL_INVALID_PROGRAM
    case CL_INVALID_PROGRAM: return QLatin1String("CL_INVALID_PROGRAM");
#endif
#ifdef CL_INVALID_PROGRAM_EXECUTABLE
    case CL_INVALID_PROGRAM_EXECUTABLE: return QLatin1String("CL_INVALID_PROGRAM_EXECUTABLE");
#endif
#ifdef CL_INVALID_KERNEL_NAME
    case CL_INVALID_KERNEL_NAME: return QLatin1String("CL_INVALID_KERNEL_NAME");
#endif
#ifdef CL_INVALID_KERNEL_DEFINITION
    case CL_INVALID_KERNEL_DEFINITION: return QLatin1String("CL_INVALID_KERNEL_DEFINITION");
#endif
#ifdef CL_INVALID_KERNEL
    case CL_INVALID_KERNEL: return QLatin1String("CL_INVALID_KERNEL");
#endif
#ifdef CL_INVALID_ARG_INDEX
    case CL_INVALID_ARG_INDEX: return QLatin1String("CL_INVALID_ARG_INDEX");
#endif
#ifdef CL_INVALID_ARG_VALUE
    case CL_INVALID_ARG_VALUE: return QLatin1String("CL_INVALID_ARG_VALUE");
#endif
#ifdef CL_INVALID_ARG_SIZE
    case CL_INVALID_ARG_SIZE: return QLatin1String("CL_INVALID_ARG_SIZE");
#endif
#ifdef CL_INVALID_KERNEL_ARGS
    case CL_INVALID_KERNEL_ARGS: return QLatin1String("CL_INVALID_KERNEL_ARGS");
#endif
#ifdef CL_INVALID_WORK_DIMENSION
    case CL_INVALID_WORK_DIMENSION: return QLatin1String("CL_INVALID_WORK_DIMENSION");
#endif
#ifdef CL_INVALID_WORK_GROUP_SIZE
    case CL_INVALID_WORK_GROUP_SIZE: return QLatin1String("CL_INVALID_WORK_GROUP_SIZE");
#endif
#ifdef CL_INVALID_WORK_ITEM_SIZE
    case CL_INVALID_WORK_ITEM_SIZE: return QLatin1String("CL_INVALID_WORK_ITEM_SIZE");
#endif
#ifdef CL_INVALID_GLOBAL_OFFSET
    case CL_INVALID_GLOBAL_OFFSET: return QLatin1String("CL_INVALID_GLOBAL_OFFSET");
#endif
#ifdef CL_INVALID_EVENT_WAIT_LIST
    case CL_INVALID_EVENT_WAIT_LIST: return QLatin1String("CL_INVALID_EVENT_WAIT_LIST");
#endif
#ifdef CL_INVALID_EVENT
    case CL_INVALID_EVENT: return QLatin1String("CL_INVALID_EVENT");
#endif
#ifdef CL_INVALID_OPERATION
    case CL_INVALID_OPERATION: return QLatin1String("CL_INVALID_OPERATION");
#endif
#ifdef CL_INVALID_GL_OBJECT
    case CL_INVALID_GL_OBJECT: return QLatin1String("CL_INVALID_GL_OBJECT");
#endif
#ifdef CL_INVALID_BUFFER_SIZE
    case CL_INVALID_BUFFER_SIZE: return QLatin1String("CL_INVALID_BUFFER_SIZE");
#endif
#ifdef CL_INVALID_MIP_LEVEL
    case CL_INVALID_MIP_LEVEL: return QLatin1String("CL_INVALID_MIP_LEVEL");
#endif
#ifdef CL_INVALID_GLOBAL_WORK_SIZE
    case CL_INVALID_GLOBAL_WORK_SIZE: return QLatin1String("CL_INVALID_GLOBAL_WORK_SIZE");
#endif

    // OpenCL-OpenGL sharing extension error codes.
    case -1000: return QLatin1String("CL_INVALID_CL_SHAREGROUP_REFERENCE_KHR");

    default: break;
    }
    return QLatin1String("Error ") + QString::number(code);
}

/*!
    Returns the context's active command queue, which will be
    defaultCommandQueue() if the queue has not yet been set.

    \sa setCommandQueue(), defaultCommandQueue()
*/
QCLCommandQueue QCLContext::commandQueue()
{
    Q_D(QCLContext);
    if (!d->commandQueue.isNull())
        return d->commandQueue;
    else
        return defaultCommandQueue();
}

/*!
    Sets the context's active command \a queue.  If \a queue is
    null, then defaultCommandQueue() will be used.

    \sa commandQueue(), defaultCommandQueue()
*/
void QCLContext::setCommandQueue(const QCLCommandQueue &queue)
{
    Q_D(QCLContext);
    d->commandQueue = queue;
}

/*!
    Returns the default command queue for defaultDevice().  If the queue
    has not been created, it will be created with the default properties
    of in-order execution of commands.

    Out of order execution can be set on the default command queue with
    QCLCommandQueue::setOutOfOrder().

    \sa commandQueue(), createCommandQueue(), lastError()
*/
QCLCommandQueue QCLContext::defaultCommandQueue()
{
    Q_D(QCLContext);
    if (d->defaultCommandQueue.isNull()) {
        if (!d->isCreated)
            return QCLCommandQueue();
        QCLDevice dev = defaultDevice();
        if (dev.isNull())
            return QCLCommandQueue();
        cl_command_queue queue;
        cl_int error = CL_INVALID_VALUE;
        queue = clCreateCommandQueue(d->id, dev.id(), 0, &error);
        d->lastError = error;
        if (!queue) {
            qWarning() << "QCLContext::defaultCommandQueue:"
                       << errorName(error);
            return QCLCommandQueue();
        }
        d->defaultCommandQueue = QCLCommandQueue(this, queue);
    }
    return d->defaultCommandQueue;
}

// Returns the active queue handle without incurring retain/release overhead.
cl_command_queue QCLContext::activeQueue()
{
    Q_D(QCLContext);
    cl_command_queue queue = d->commandQueue.id();
    if (queue)
        return queue;
    queue = d->defaultCommandQueue.id();
    if (queue)
        return queue;
    return defaultCommandQueue().id();
}

/*!
    Creates a new command queue on this context for \a device and
    \a properties.

    Unlike defaultCommandQueue(), this function will create a new queue
    every time it is called.  The queue will be deleted when the last
    reference to the returned object is removed.

    \sa defaultCommandQueue(), lastError()
*/
QCLCommandQueue QCLContext::createCommandQueue
    (const QCLDevice &device, cl_command_queue_properties properties)
{
    Q_D(QCLContext);
    cl_command_queue queue;
    cl_int error = CL_INVALID_VALUE;
    queue = clCreateCommandQueue(d->id, device.id(), properties, &error);
    reportError("QCLContext::createCommandQueue:", error);
    if (queue)
        return QCLCommandQueue(this, queue);
    else
        return QCLCommandQueue();
}

/*!
    Creates an OpenCL memory buffer of \a size bytes in length,
    with the specified \a access mode.

    The memory is created on the device and will not be accessible
    to the host via a direct pointer.  Use createBufferHost() to
    create a host-accessible buffer.

    Returns the new OpenCL memory buffer object, or a null object
    if the buffer could not be created.

    \sa createBufferHost(), createBufferCopy(), createVector()
*/
QCLBuffer QCLContext::createBufferDevice(size_t size, QCL::Access access)
{
    Q_D(QCLContext);
    cl_int error = CL_INVALID_CONTEXT;
    cl_mem_flags flags = cl_mem_flags(access);
    cl_mem mem = clCreateBuffer(d->id, flags, size, 0, &error);
    reportError("QCLContext::createBufferDevice:", error);
    if (mem)
        return QCLBuffer(this, mem);
    else
        return QCLBuffer();
}

/*!
    Creates an OpenCL memory buffer of \a size bytes in length,
    with the specified \a access mode.

    If \a data is not null, then it will be used as the storage
    for the buffer.  If \a data is null, then a new block of
    host-accessible memory will be allocated.

    Returns the new OpenCL memory buffer object, or a null object
    if the buffer could not be created.

    \sa createBufferDevice(), createBufferCopy(), createVector()
*/
QCLBuffer QCLContext::createBufferHost
    (void *data, size_t size, QCL::Access access)
{
    Q_D(QCLContext);
    cl_int error = CL_INVALID_CONTEXT;
    cl_mem_flags flags = cl_mem_flags(access);
    if (data)
        flags |= CL_MEM_USE_HOST_PTR;
    else
        flags |= CL_MEM_ALLOC_HOST_PTR;
    cl_mem mem = clCreateBuffer(d->id, flags, size, data, &error);
    reportError("QCLContext::createBufferHost:", error);
    if (mem)
        return QCLBuffer(this, mem);
    else
        return QCLBuffer();
}

/*!
    Creates an OpenCL memory buffer of \a size bytes in length,
    with the specified \a access mode.

    The buffer is initialized with a copy of the contents of \a data.
    The application's \a data can be discarded after the buffer
    is created.

    Returns the new OpenCL memory buffer object, or a null object
    if the buffer could not be created.

    \sa createBufferDevice(), createBufferHost(), createVector()
*/
QCLBuffer QCLContext::createBufferCopy
    (const void *data, size_t size, QCL::Access access)
{
    Q_ASSERT(data);
    Q_D(QCLContext);
    cl_int error = CL_INVALID_CONTEXT;
    cl_mem_flags flags = cl_mem_flags(access);
    flags |= CL_MEM_COPY_HOST_PTR;
    cl_mem mem = clCreateBuffer
        (d->id, flags, size, const_cast<void *>(data), &error);
    reportError("QCLContext::createBufferCopy:", error);
    if (mem)
        return QCLBuffer(this, mem);
    else
        return QCLBuffer();
}

/*!
    \fn QCLVector<T> QCLContext::createVector(int size, QCL::Access access)

    Creates a host-accessible vector of \a size elements of type T
    on this context and returns it.  The elements will be initially in
    an undefined state.

    Note that the \a access mode indicates how the OpenCL device (e.g. GPU)
    will access the vector.  When the host maps the vector, it will always
    be mapped as ReadWrite.

    \sa createBufferHost()
*/

/*!
    Creates a 2D OpenCL image object with the specified \a format,
    \a size, and \a access mode.

    The image memory is created on the device and will not be accessible
    to the host via a direct pointer.  Use createImage2DHost() to
    create a host-accessible image.

    Returns the new 2D OpenCL image object, or a null object
    if the image could not be created.

    \sa createImage2DHost(), createImage2DCopy()
*/
QCLImage2D QCLContext::createImage2DDevice
    (const QCLImageFormat &format, const QSize &size, QCL::Access access)
{
    Q_D(QCLContext);
    cl_int error = CL_INVALID_CONTEXT;
    cl_mem_flags flags = cl_mem_flags(access);
    cl_mem mem = clCreateImage2D
        (d->id, flags, &(format.m_format), size.width(), size.height(), 0,
         0, &error);
    reportError("QCLContext::createImage2DDevice:", error);
    if (mem)
        return QCLImage2D(this, mem);
    else
        return QCLImage2D();
}

/*!
    Creates a 2D OpenCL image object with the specified \a format,
    \a size, and \a access mode.  If \a bytesPerLine is not zero,
    it indicates the number of bytes between lines in \a data.

    If \a data is not null, then it will be used as the storage
    for the image.  If \a data is null, then a new block of
    host-accessible memory will be allocated.

    Returns the new 2D OpenCL image object, or a null object
    if the image could not be created.

    \sa createImage2DDevice(), createImage2DCopy()
*/
QCLImage2D QCLContext::createImage2DHost
    (const QCLImageFormat &format, void *data, const QSize &size,
     QCL::Access access, int bytesPerLine)
{
    Q_D(QCLContext);
    cl_int error = CL_INVALID_CONTEXT;
    cl_mem_flags flags = cl_mem_flags(access);
    if (data)
        flags |= CL_MEM_USE_HOST_PTR;
    else
        flags |= CL_MEM_ALLOC_HOST_PTR;
    cl_mem mem = clCreateImage2D
        (d->id, flags, &(format.m_format),
         size.width(), size.height(), bytesPerLine,
         data, &error);
    reportError("QCLContext::createImage2DHost:", error);
    if (mem)
        return QCLImage2D(this, mem);
    else
        return QCLImage2D();
}

/*!
    Creates a 2D OpenCL image object from \a image with the
    specified \a access mode.

    OpenCL kernels that access the image will read and write
    the QImage contents directly.

    Returns the new 2D OpenCL image object, or a null object
    if the image could not be created.  If \a image is null or
    has a zero size, this function will return a null
    QCLImage2D object.

    \sa createImage2DDevice(), createImage2DCopy()
*/
QCLImage2D QCLContext::createImage2DHost(QImage *image, QCL::Access access)
{
    Q_D(QCLContext);

    // Validate the image.
    if (!image || image->width() < 1 || image->height() < 1)
        return QCLImage2D();
    QCLImageFormat format(image->format());
    if (format.isNull()) {
        qWarning("QCLContext::createImage2DHost: QImage format %d "
                 "does not have an OpenCL equivalent", int(image->format()));
        return QCLImage2D();
    }

    // Create the image object.
    cl_int error = CL_INVALID_CONTEXT;
    cl_mem_flags flags = cl_mem_flags(access) | CL_MEM_USE_HOST_PTR;
    cl_mem mem = clCreateImage2D
        (d->id, flags, &(format.m_format),
         image->width(), image->height(), image->bytesPerLine(),
         image->bits(), &error);
    reportError("QCLContext::createImage2DHost:", error);
    if (mem)
        return QCLImage2D(this, mem);
    else
        return QCLImage2D();
}

/*!
    Creates a 2D OpenCL image object with the specified \a format,
    \a size, and \a access mode.  If \a bytesPerLine is not zero,
    it indicates the number of bytes between lines in \a data.

    The image is initialized with a copy of the contents of \a data.
    The application's \a data can be discarded after the image
    is created.

    Returns the new 2D OpenCL image object, or a null object
    if the image could not be created.

    \sa createImage2DDevice(), createImage2DHost()
*/
QCLImage2D QCLContext::createImage2DCopy
    (const QCLImageFormat &format, const void *data, const QSize &size,
     QCL::Access access, int bytesPerLine)
{
    Q_D(QCLContext);
    cl_int error = CL_INVALID_CONTEXT;
    cl_mem_flags flags = cl_mem_flags(access) | CL_MEM_COPY_HOST_PTR;
    cl_mem mem = clCreateImage2D
        (d->id, flags, &(format.m_format),
         size.width(), size.height(), bytesPerLine,
         const_cast<void *>(data), &error);
    reportError("QCLContext::createImage2DCopy:", error);
    if (mem)
        return QCLImage2D(this, mem);
    else
        return QCLImage2D();
}

/*!
    Creates a 2D OpenCL image object from \a image with the
    specified \a access mode.

    The OpenCL image is initialized with a copy of the contents of
    \a image.  The application's \a image can be discarded after the
    OpenCL image is created.

    Returns the new 2D OpenCL image object, or a null object
    if the image could not be created.  If \a image has a zero size,
    this function will return a null QCLImage2D object.

    \sa createImage2DDevice(), createImage2DHost()
*/
QCLImage2D QCLContext::createImage2DCopy
    (const QImage &image, QCL::Access access)
{
    Q_D(QCLContext);

    // Validate the image.
    if (image.width() < 1 || image.height() < 1)
        return QCLImage2D();
    QCLImageFormat format(image.format());
    if (format.isNull()) {
        qWarning("QCLContext::createImage2DCopy: QImage format %d "
                 "does not have an OpenCL equivalent", int(image.format()));
        return QCLImage2D();
    }

    // Create the image object.
    cl_int error = CL_INVALID_CONTEXT;
    cl_mem_flags flags = cl_mem_flags(access) | CL_MEM_COPY_HOST_PTR;
    cl_mem mem = clCreateImage2D
        (d->id, flags, &(format.m_format),
         image.width(), image.height(), image.bytesPerLine(),
         const_cast<uchar *>(image.bits()), &error);
    reportError("QCLContext::createImage2DCopy:", error);
    if (mem)
        return QCLImage2D(this, mem);
    else
        return QCLImage2D();
}

/*!
    Creates a 3D OpenCL image object with the specified \a format,
    \a width, \a height, \a depth, and \a access mode.

    The image memory is created on the device and will not be accessible
    to the host via a direct pointer.  Use createImage3DHost() to
    create a host-accessible image.

    Returns the new 3D OpenCL image object, or a null object
    if the image could not be created.

    \sa createImage3DHost(), createImage3DCopy()
*/
QCLImage3D QCLContext::createImage3DDevice
    (const QCLImageFormat &format, int width, int height, int depth,
     QCL::Access access)
{
    Q_D(QCLContext);
    cl_int error = CL_INVALID_CONTEXT;
    cl_mem_flags flags = cl_mem_flags(access);
    cl_mem mem = clCreateImage3D
        (d->id, flags, &(format.m_format), width, height, depth, 0, 0,
         0, &error);
    reportError("QCLContext::createImage3DDevice:", error);
    if (mem)
        return QCLImage3D(this, mem);
    else
        return QCLImage3D();
}

/*!
    Creates a 3D OpenCL image object with the specified \a format,
    \a width, \a height, \a depth, and \a access mode.
    If \a bytesPerLine is not zero, it indicates the number of
    bytes between lines in \a data.  If \a bytesPerSlice is not zero,
    it indicates the number of bytes between slices in \a data.

    If \a data is not null, then it will be used as the storage
    for the image.  If \a data is null, then a new block of
    host-accessible memory will be allocated.

    Returns the new 3D OpenCL image object, or a null object
    if the image could not be created.

    \sa createImage3DDevice(), createImage3DCopy()
*/
QCLImage3D QCLContext::createImage3DHost
    (const QCLImageFormat &format, void *data,
     int width, int height, int depth, QCL::Access access,
     int bytesPerLine, int bytesPerSlice)
{
    Q_D(QCLContext);
    cl_int error = CL_INVALID_CONTEXT;
    cl_mem_flags flags = cl_mem_flags(access);
    if (data)
        flags |= CL_MEM_USE_HOST_PTR;
    else
        flags |= CL_MEM_ALLOC_HOST_PTR;
    cl_mem mem = clCreateImage3D
        (d->id, flags, &(format.m_format),
         width, height, depth, bytesPerLine, bytesPerSlice,
         data, &error);
    reportError("QCLContext::createImage3DHost:", error);
    if (mem)
        return QCLImage3D(this, mem);
    else
        return QCLImage3D();
}

/*!
    Creates a 3D OpenCL image object with the specified \a format,
    \a width, \a height, \a depth, and \a access mode.
    If \a bytesPerLine is not zero, it indicates the number of bytes
    between lines in \a data.  If \a bytesPerSlice is not zero,
    it indicates the number of bytes between slices in \a data.

    The image is initialized with a copy of the contents of \a data.
    The application's \a data can be discarded after the image
    is created.

    Returns the new 3D OpenCL image object, or a null object
    if the image could not be created.

    \sa createImage3DDevice(), createImage3DHost()
*/
QCLImage3D QCLContext::createImage3DCopy
    (const QCLImageFormat &format, const void *data,
     int width, int height, int depth,
     QCL::Access access, int bytesPerLine, int bytesPerSlice)
{
    Q_D(QCLContext);
    cl_int error = CL_INVALID_CONTEXT;
    cl_mem_flags flags = cl_mem_flags(access) | CL_MEM_COPY_HOST_PTR;
    cl_mem mem = clCreateImage3D
        (d->id, flags, &(format.m_format),
         width, height, depth, bytesPerLine, bytesPerSlice,
         const_cast<void *>(data), &error);
    reportError("QCLContext::createImage3DCopy:", error);
    if (mem)
        return QCLImage3D(this, mem);
    else
        return QCLImage3D();
}

/*!
    Creates and returns an OpenCL sampler from the \a normalizedCoords,
    \a addressingMode, and \a filterMode parameters.
*/
QCLSampler QCLContext::createSampler
    (bool normalizedCoords, cl_addressing_mode addressingMode,
     cl_filter_mode filterMode)
{
    Q_D(QCLContext);
    cl_int error;
    cl_sampler sampler = clCreateSampler
        (d->id, normalizedCoords ? CL_TRUE : CL_FALSE,
         addressingMode, filterMode, &error);
    reportError("QCLContext::createSampler:", error);
    if (sampler)
        return QCLSampler(sampler);
    else
        return QCLSampler();
}

/*!
    Creates an OpenCL program object from the supplied \a sourceCode.

    \sa createProgramFromSourceFile(), buildProgramFromSourceCode()
*/
QCLProgram QCLContext::createProgramFromSourceCode(const QByteArray &sourceCode)
{
    Q_D(QCLContext);
    Q_ASSERT(!sourceCode.isEmpty());
    cl_int error = CL_INVALID_CONTEXT;
    const char *code = sourceCode.constData();
    size_t length = sourceCode.size();
    cl_program prog = clCreateProgramWithSource
        (d->id, 1, &code, &length, &error);
    reportError("QCLContext::createProgramFromSourceCode(QByteArray):", error);
    if (prog)
        return QCLProgram(this, prog);
    else
        return QCLProgram();
}

/*!
    Creates an OpenCL program object from the contents of the specified
    \a fileName.

    \sa createProgramFromSourceCode(), buildProgramFromSourceFile()
*/
QCLProgram QCLContext::createProgramFromSourceFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        qWarning() << "QCLContext::createProgramFromSourceFile: Unable to open file" << fileName;
        return QCLProgram();
    }
    QByteArray contents = file.readAll();
    return createProgramFromSourceCode(contents.constData());
}

/*!
    Creates an OpenCL program object from the supplied \a sourceCode
    and then builds it.  Returns a null QCLProgram if the program
    could not be built.

    \sa createProgramFromSourceCode(), buildProgramFromSourceFile()
*/
QCLProgram QCLContext::buildProgramFromSourceCode(const QByteArray &sourceCode)
{
    QCLProgram program = createProgramFromSourceCode(sourceCode);
    if (program.isNull() || program.build())
        return program;
    return QCLProgram();
}

/*!
    Creates an OpenCL program object from the contents of the supplied
    \a fileName and then builds it.  Returns a null QCLProgram if the
    program could not be built.

    \sa createProgramFromSourceFile(), buildProgramFromSourceCode()
*/
QCLProgram QCLContext::buildProgramFromSourceFile(const QString &fileName)
{
    QCLProgram program = createProgramFromSourceFile(fileName);
    if (program.isNull() || program.build())
        return program;
    return QCLProgram();
}

static QList<QCLImageFormat> qt_cl_supportedImageFormats
    (cl_context ctx, cl_mem_flags flags, cl_mem_object_type image_type)
{
    cl_uint count = 0;
    QList<QCLImageFormat> list;
    if (clGetSupportedImageFormats
            (ctx, flags, image_type,
             0, 0, &count) != CL_SUCCESS || !count)
        return list;
    QVarLengthArray<cl_image_format> buf(count);
    if (clGetSupportedImageFormats
            (ctx, flags, image_type,
             count, buf.data(), 0) != CL_SUCCESS)
        return list;
    for (cl_uint index = 0; index < count; ++index) {
        list.append(QCLImageFormat
            (QCLImageFormat::ChannelOrder(buf[index].image_channel_order),
             QCLImageFormat::ChannelType(buf[index].image_channel_data_type)));
    }
    return list;
}

/*!
    Returns the list of supported 2D image formats for processing
    images with the specified memory \a flags.

    \sa supportedImage3DFormats()
*/
QList<QCLImageFormat> QCLContext::supportedImage2DFormats
    (cl_mem_flags flags) const
{
    Q_D(const QCLContext);
    return qt_cl_supportedImageFormats(d->id, flags, CL_MEM_OBJECT_IMAGE2D);
}

/*!
    Returns the list of supported 3D image formats for processing
    images with the specified memory \a flags.

    \sa supportedImage2DFormats()
*/
QList<QCLImageFormat> QCLContext::supportedImage3DFormats
    (cl_mem_flags flags) const
{
    Q_D(const QCLContext);
    return qt_cl_supportedImageFormats(d->id, flags, CL_MEM_OBJECT_IMAGE3D);
}

/*!
    Flushes all previously queued commands to the device associated
    with the active command queue.  The commands are delivered to
    the device, but no guarantees are given that they will be executed.

    \sa finish()
*/
void QCLContext::flush()
{
    clFlush(activeQueue());
}

/*!
    Blocks until all previously queued commands on the active
    command queue have finished execution.

    \sa flush()
*/
void QCLContext::finish()
{
    clFinish(activeQueue());
}

/*!
    Returns a marker event for the active command queue.  The event
    will be signalled when all commands that were queued before this
    point have finished.

    \sa barrier(), sync()
*/
QCLEvent QCLContext::marker()
{
    cl_event evid;
    cl_int error = clEnqueueMarker(activeQueue(), &evid);
    reportError("QCLContext::marker:", error);
    if (error != CL_SUCCESS)
        return QCLEvent();
    else
        return QCLEvent(evid);
}

/*!
    Synchronizes the host against the active command queue.
    This function will block until all currently queued commands
    have finished execution.
*/
void QCLContext::sync()
{
    cl_event event;
    cl_int error = clEnqueueMarker(activeQueue(), &event);
    reportError("QCLContext::sync:", error);
    if (error == CL_SUCCESS) {
        clWaitForEvents(1, &event);
        clReleaseEvent(event);
    }
}

/*!
    Adds a barrier to the active command queue.  All commands that
    were queued before this point must finish before any further
    commands added after this point are executed.

    This function will return immediately and will not block waiting
    for the commands to finish.  Use sync() to block until all queued
    commands finish.

    \sa marker(), sync()
*/
void QCLContext::barrier()
{
    cl_int error = clEnqueueBarrier(activeQueue());
    reportError("QCLContext::barrier:", error);
}

/*!
    \overload

    Adds a barrier to the active command queue that will prevent future
    commands from being executed until after all members of \a events
    have been signalled.

    \sa marker()
*/
void QCLContext::barrier(const QVector<QCLEvent> &events)
{
    if (events.isEmpty())
        return;
    cl_int error = clEnqueueWaitForEvents
        (activeQueue(), events.size(),
         reinterpret_cast<const cl_event *>(events.constData()));
    reportError("QCLContext::barrier(QVector<QCLEvent>):", error);
}

/*!
    \internal
*/
void QCLContext::reportError(const char *name, cl_int error)
{
    Q_D(QCLContext);
    d->lastError = error;
    if (error != CL_SUCCESS)
        qWarning() << name << errorName(error);
}

QT_END_NAMESPACE
