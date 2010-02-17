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

#include "qclkernel.h"
#include "qclprogram.h"
#include "qclbuffer.h"
#include "qclcontext.h"
#include <QtCore/qvarlengtharray.h>

QT_BEGIN_NAMESPACE

/*!
    \class QCLKernel
    \brief The QCLKernel class represents an executable entry point function in an OpenCL program.
    \since 4.7
    \ingroup opencl

    QCLKernel corresponds to an instance of an OpenCL kernel, decorated
    with a specific globalWorkSize() and localWorkSize().  It is possible
    to use the same OpenCL kernel with different work sizes by altering
    the size for each execution request:

    \code
    QCLKernel kernel = program.createKernel("foo");

    kernel.setGlobalWorkSize(100, 100);
    kernel.setArg(0, a1);
    kernel.setArg(1, b1);
    kernel.execute();

    kernel.setGlobalWorkSize(200, 200);
    kernel.setArg(0, a2);
    kernel.setArg(1, b2);
    kernel.execute();
    \endcode

    Alternatively, operator()() can be used to avoid the setArg() calls:

    \code
    QCLKernel kernel = program.createKernel("foo");

    kernel.setGlobalWorkSize(100, 100);
    kernel(a1, b1);

    kernel.setGlobalWorkSize(200, 200);
    kernel(a2, b2);
    \endcode

    Up to 10 arguments can be provided to operator()().  Use explicit
    setArg() and execute() calls with kernels that have more than
    10 arguments.

    Note that both execute() and operator()() return immediately;
    they will not block until execution is complete.  Both functions
    return a QCLEvent object that can be used to wait for the
    request to complete:

    \code
    kernel.setGlobalWorkSize(100, 100);
    QCLEvent event = kernel(a1, b1);
    event.wait();
    \endcode

    Usually it isn't necessary for an explicit QCLEvent wait
    because the next OpenCL request will implicitly block until
    the kernel completes execution:

    \code
    QCLBuffer buffer = ...;

    kernel.setGlobalWorkSize(100, 100);
    kernel(buffer);

    buffer.read(...);
    \endcode

    With the default in-order command execution policy, OpenCL will ensure
    that the QCLBuffer::read() request will not begin execution until the
    kernel execution completes.

    \sa QCLProgram
*/

class QCLKernelPrivate
{
public:
    QCLKernelPrivate(QCLContext *ctx, cl_kernel kid)
        : context(ctx)
        , id(kid)
        , globalWorkSize(1, 0, 0)
        , localWorkSize(0, 0, 0)
    {}
    QCLKernelPrivate(const QCLKernelPrivate *other)
        : context(other->context)
        , id(other->id)
        , globalWorkSize(other->globalWorkSize)
        , localWorkSize(other->localWorkSize)
    {
        if (id)
            clRetainKernel(id);
    }
    ~QCLKernelPrivate()
    {
        if (id)
            clReleaseKernel(id);
    }

    void copy(const QCLKernelPrivate *other)
    {
        context = other->context;
        globalWorkSize = other->globalWorkSize;
        localWorkSize = other->localWorkSize;
        if (id != other->id) {
            if (id)
                clReleaseKernel(id);
            id = other->id;
            if (id)
                clRetainKernel(id);
        }
    }

    QCLContext *context;
    cl_kernel id;
    QCLWorkSize globalWorkSize;
    QCLWorkSize localWorkSize;
};

/*!
    Constructs a null OpenCL kernel object.
*/
QCLKernel::QCLKernel()
    : d_ptr(new QCLKernelPrivate(0, 0))
{
}

/*!
    Constructs an OpenCL kernel object from the native identifier \a id,
    and associates it with \a context.  This class will take over
    ownership of \a id and release it in the destructor.
*/
QCLKernel::QCLKernel(QCLContext *context, cl_kernel id)
    : d_ptr(new QCLKernelPrivate(context, id))
{
}

/*!
    Constructs a copy of \a other.
*/
QCLKernel::QCLKernel(const QCLKernel& other)
    : d_ptr(new QCLKernelPrivate(other.d_ptr.data()))
{
}

/*!
    Releases this OpenCL kernel object.  If this is the last
    reference to the kernel, it will be destroyed.
*/
QCLKernel::~QCLKernel()
{
}

/*!
    Assigns \a other to this object.
*/
QCLKernel& QCLKernel::operator=(const QCLKernel& other)
{
    d_ptr->copy(other.d_ptr.data());
    return *this;
}

/*!
    Returns true if this OpenCL kernel object is null; false otherwise.
*/
bool QCLKernel::isNull() const
{
    Q_D(const QCLKernel);
    return d->id == 0;
}

/*!
    Returns the native OpenCL identifier for this kernel.
*/
cl_kernel QCLKernel::id() const
{
    Q_D(const QCLKernel);
    return d->id;
}

/*!
    Returns the OpenCL context that this kernel was created within.
*/
QCLContext *QCLKernel::context() const
{
    Q_D(const QCLKernel);
    return d->context;
}

/*!
    Returns the OpenCL program that this kernel is associated with.
*/
QCLProgram QCLKernel::program() const
{
    Q_D(const QCLKernel);
    if (!d->id)
        return QCLProgram();
    cl_program prog = 0;
    if (clGetKernelInfo(d->id, CL_KERNEL_PROGRAM,
                        sizeof(prog), &prog, 0) != CL_SUCCESS)
        return QCLProgram();
    return QCLProgram(d->context, prog);
}

/*!
    Returns the name of this OpenCL kernel's entry point function.
*/
QString QCLKernel::name() const
{
    Q_D(const QCLKernel);
    size_t size = 0;
    if (clGetKernelInfo(d->id, CL_KERNEL_FUNCTION_NAME,
                        0, 0, &size) != CL_SUCCESS || !size)
        return QString();
    QVarLengthArray<char> buf(size);
    if (clGetKernelInfo(d->id, CL_KERNEL_FUNCTION_NAME,
                        size, buf.data(), 0) != CL_SUCCESS)
        return QString();
    return QString::fromLatin1(buf.constData(), size);
}

/*!
    Returns the number of arguments that are expected by
    this OpenCL kernel.

    \sa setArg()
*/
int QCLKernel::argCount() const
{
    Q_D(const QCLKernel);
    cl_uint count = 0;
    if (clGetKernelInfo(d->id, CL_KERNEL_NUM_ARGS, sizeof(count), &count, 0)
            != CL_SUCCESS)
        return 0;
    return int(count);
}

/*!
    Returns the work group size that was declared in the kernel's
    source code using a \c{reqd_work_group_size} qualifier.
    Returns (0, 0, 0) if the size is not declared.

    The default device for context() is used to retrieve the
    work group size.
*/
QCLWorkSize QCLKernel::declaredWorkGroupSize() const
{
    Q_D(const QCLKernel);
    size_t sizes[3];
    if (clGetKernelWorkGroupInfo
            (d->id, d->context->defaultDevice().id(),
             CL_KERNEL_COMPILE_WORK_GROUP_SIZE,
             sizeof(sizes), sizes, 0) != CL_SUCCESS)
        return QCLWorkSize(0, 0, 0);
    else
        return QCLWorkSize(sizes[0], sizes[1], sizes[2]);
}

/*!
    \overload

    Returns the work group size that was declared in the kernel's
    source code using a \c{reqd_work_group_size} qualifier.
    Returns (0, 0, 0) if the size is not declared.

    The specified \a device is used to retrieve the work group size.
*/
QCLWorkSize QCLKernel::declaredWorkGroupSize(const QCLDevice& device) const
{
    Q_D(const QCLKernel);
    size_t sizes[3];
    if (clGetKernelWorkGroupInfo
            (d->id, device.id(),
             CL_KERNEL_COMPILE_WORK_GROUP_SIZE,
             sizeof(sizes), sizes, 0) != CL_SUCCESS)
        return QCLWorkSize(0, 0, 0);
    else
        return QCLWorkSize(sizes[0], sizes[1], sizes[2]);
}

/*!
    Returns the global work size for this instance of the kernel.
    The default value is (1, 0, 0).

    \sa setGlobalWorkSize(), localWorkSize()
*/
QCLWorkSize QCLKernel::globalWorkSize() const
{
    Q_D(const QCLKernel);
    return d->globalWorkSize;
}

/*!
    Sets the global work size for this instance of the kernel to \a size.

    \sa globalWorkSize(), setLocalWorkSize()
*/
void QCLKernel::setGlobalWorkSize(const QCLWorkSize& size)
{
    Q_D(QCLKernel);
    d->globalWorkSize = size;
}

/*!
    \fn void QCLKernel::setGlobalWorkSize(size_t width, size_t height)
    \overload

    Sets the global work size for this instance of the kernel to
    \a width x \a height.
*/

/*!
    \fn void QCLKernel::setGlobalWorkSize(size_t width, size_t height, size_t depth)
    \overload

    Sets the global work size for this instance of the kernel to
    \a width x \a height x \a depth.
*/

/*!
    \fn void QCLKernel::setLocalWorkSize(size_t width, size_t height)
    \overload

    Sets the local work size for this instance of the kernel to
    \a width x \a height.
*/

/*!
    \fn void QCLKernel::setLocalWorkSize(size_t width, size_t height, size_t depth)
    \overload

    Sets the local work size for this instance of the kernel to
    \a width x \a height x \a depth.
*/

/*!
    Returns the local work size for this instance of the kernel.
    The default value is (0, 0, 0), which indicates that the local
    work size is not used.

    \sa setLocalWorkSize(), globalWorkSize()
*/
QCLWorkSize QCLKernel::localWorkSize() const
{
    Q_D(const QCLKernel);
    return d->localWorkSize;
}

/*!
    Sets the local work size for this instance of the kernel to \a size.

    \sa localWorkSize(), setGlobalWorkSize()
*/
void QCLKernel::setLocalWorkSize(const QCLWorkSize& size)
{
    Q_D(QCLKernel);
    d->localWorkSize = size;
}

/*!
    Sets argument \a index for this kernel to \a value.
*/
void QCLKernel::setArg(int index, cl_int value)
{
    Q_D(const QCLKernel);
    clSetKernelArg(d->id, index, sizeof(value), &value);
}

/*!
    Sets argument \a index for this kernel to \a value.
*/
void QCLKernel::setArg(int index, cl_uint value)
{
    Q_D(const QCLKernel);
    clSetKernelArg(d->id, index, sizeof(value), &value);
}

/*!
    Sets argument \a index for this kernel to \a value.
*/
void QCLKernel::setArg(int index, cl_long value)
{
    Q_D(const QCLKernel);
    clSetKernelArg(d->id, index, sizeof(value), &value);
}

/*!
    Sets argument \a index for this kernel to \a value.
*/
void QCLKernel::setArg(int index, cl_ulong value)
{
    Q_D(const QCLKernel);
    clSetKernelArg(d->id, index, sizeof(value), &value);
}

/*!
    Sets argument \a index for this kernel to \a value.
*/
void QCLKernel::setArg(int index, float value)
{
    Q_D(const QCLKernel);
    clSetKernelArg(d->id, index, sizeof(value), &value);
}

/*!
    Sets argument \a index for this kernel to \a value.
*/
void QCLKernel::setArg(int index, const QCLMemoryObject& value)
{
    Q_D(const QCLKernel);
    cl_mem id = value.id();
    clSetKernelArg(d->id, index, sizeof(id), &id);
}

/*!
    Sets argument \a index for this kernel to \a value.
*/
void QCLKernel::setArg(int index, const QCLSampler& value)
{
    Q_D(const QCLKernel);
    cl_sampler id = value.id();
    clSetKernelArg(d->id, index, sizeof(id), &id);
}

/*!
    Sets argument \a index to the \a size bytes at \a data.
*/
void QCLKernel::setArg(int index, const void *data, size_t size)
{
    Q_D(const QCLKernel);
    clSetKernelArg(d->id, index, size, data);
}

/*!
    Requests that this kernel instance be executed on globalWorkSize() items,
    optionally subdivided into work groups of localWorkSize() items.

    Returns an event object that can be use to wait for the kernel
    to finish execution.  The request is executed on the active
    command queue for context().

    \sa operator()()
*/
QCLEvent QCLKernel::execute()
{
    Q_D(const QCLKernel);
    cl_event event;
    cl_int error = clEnqueueNDRangeKernel
        (d->context->activeQueue(), d->id, d->globalWorkSize.dimensions(),
         0, d->globalWorkSize.sizes(),
         (d->localWorkSize.width() ? d->localWorkSize.sizes() : 0),
         0, 0, &event);
    context()->reportError("QCLKernel::execute:", error);
    if (error != CL_SUCCESS)
        return QCLEvent();
    else
        return QCLEvent(event);
}

/*!
    \overload

    Requests that this kernel instance be executed on globalWorkSize() items,
    optionally subdivided into work groups of localWorkSize() items.

    If \a after is not an empty list, it indicates the events that must
    be signalled as complete before this kernel instance can begin executing.

    Returns an event object that can be use to wait for the kernel
    to finish execution.  The request is executed on the active
    command queue for context().
*/
QCLEvent QCLKernel::execute(const QVector<QCLEvent>& after)
{
    Q_D(const QCLKernel);
    cl_event event;
    cl_int error = clEnqueueNDRangeKernel
        (d->context->activeQueue(), d->id, d->globalWorkSize.dimensions(),
         0, d->globalWorkSize.sizes(),
         (d->localWorkSize.width() ? d->localWorkSize.sizes() : 0),
         after.size(),
         (after.isEmpty() ? 0 :
            reinterpret_cast<const cl_event *>(after.constData())),
         &event);
    context()->reportError("QCLKernel::execute:", error);
    if (error != CL_SUCCESS)
        return QCLEvent();
    else
        return QCLEvent(event);
}

/*!
    \fn QCLEvent QCLKernel::operator()()

    Executes this kernel instance with zero arguments.
    Returns an event object that can be used to wait for the
    kernel to complete execution.
*/

/*!
    \fn QCLEvent QCLKernel::operator()(const T1& arg1)

    Executes this kernel instance with the argument \a arg1.
    Returns an event object that can be used to wait for the
    kernel to complete execution.
*/

/*!
    \fn QCLEvent QCLKernel::operator()(const T1& arg1, const T2& arg2)

    Executes this kernel instance with the arguments \a arg1 and \a arg2.
    Returns an event object that can be used to wait for the
    kernel to complete execution.
*/

/*!
    \fn QCLEvent QCLKernel::operator()(const T1& arg1, const T2& arg2, const T3& arg3)

    Executes this kernel instance with the arguments \a arg1, \a arg2,
    and \a arg3.  Returns an event object that can be used to wait for the
    kernel to complete execution.
*/

/*!
    \fn QCLEvent QCLKernel::operator()(const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4)

    Executes this kernel instance with the arguments \a arg1, \a arg2,
    \a arg3, and \a arg4.  Returns an event object that can be used to
    wait for the kernel to complete execution.
*/

/*!
    \fn QCLEvent QCLKernel::operator()(const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4, const T5& arg5)

    Executes this kernel instance with the arguments \a arg1, \a arg2,
    \a arg3, \a arg4, and \a arg5.  Returns an event object that can be
    used to wait for the kernel to complete execution.
*/

/*!
    \fn QCLEvent QCLKernel::operator()(const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4, const T5& arg5, const T6& arg6)

    Executes this kernel instance with the arguments \a arg1, \a arg2,
    \a arg3, \a arg4, \a arg5, and \a arg6.  Returns an event object that
    can be used to wait for the kernel to complete execution.
*/

/*!
    \fn QCLEvent QCLKernel::operator()(const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4, const T5& arg5, const T6& arg6, const T7& arg7)

    Executes this kernel instance with the arguments \a arg1, \a arg2,
    \a arg3, \a arg4, \a arg5, \a arg6, and \a arg7.  Returns an event
    object that can be used to wait for the kernel to complete execution.
*/

/*!
    \fn QCLEvent QCLKernel::operator()(const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4, const T5& arg5, const T6& arg6, const T7& arg7, const T8& arg8)

    Executes this kernel instance with the arguments \a arg1, \a arg2,
    \a arg3, \a arg4, \a arg5, \a arg6, \a arg7, and \a arg8.  Returns
    an event object that can be used to wait for the kernel to complete
    execution.
*/

/*!
    \fn QCLEvent QCLKernel::operator()(const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4, const T5& arg5, const T6& arg6, const T7& arg7, const T8& arg8, const T9& arg9)

    Executes this kernel instance with the arguments \a arg1, \a arg2,
    \a arg3, \a arg4, \a arg5, \a arg6, \a arg7, \a arg8, and \a arg9.
    Returns an event object that can be used to wait for the kernel
    to complete execution.
*/

/*!
    \fn QCLEvent QCLKernel::operator()(const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4, const T5& arg5, const T6& arg6, const T7& arg7, const T8& arg8, const T9& arg9, const T10& arg10)

    Executes this kernel instance with the arguments \a arg1, \a arg2,
    \a arg3, \a arg4, \a arg5, \a arg6, \a arg7, \a arg8, \a arg9,
    and \a arg10.  Returns an event object that can be used to wait
    for the kernel to complete execution.
*/

QT_END_NAMESPACE
