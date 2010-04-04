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

#include "qclbuffer.h"
#include "qclimage.h"
#include "qclcontext.h"

QT_BEGIN_NAMESPACE

/*!
    \class QCLBuffer
    \brief The QCLBuffer class represents an OpenCL buffer object.
    \since 4.7
    \ingroup opencl

    \sa QCLVector
*/

/*!
    \fn QCLBuffer::QCLBuffer()

    Constructs a null OpenCL buffer object.
*/

/*!
    \fn QCLBuffer::QCLBuffer(QCLContext *context, cl_mem id)

    Constructs an OpenCL buffer object that is initialized with the
    native OpenCL identifier \a id, and associates it with \a context.
    This class will take over ownership of \a id and will release
    it in the destructor.
*/

/*!
    \fn QCLBuffer::QCLBuffer(const QCLBuffer &other)

    Constructs a copy of \a other.
*/

/*!
    \fn QCLBuffer &QCLBuffer::operator=(const QCLBuffer &other)

    Assigns \a other to this object.
*/

/*!
    Reads \a size bytes from this buffer, starting at \a offset,
    into the supplied \a data array.  Returns true if the read
    was successful; false otherwise.

    This function will block until the request finishes.
    The request is executed on the active command queue for context().

    \sa readAsync(), write()
*/
bool QCLBuffer::read(size_t offset, void *data, size_t size)
{
    cl_int error = clEnqueueReadBuffer
        (context()->activeQueue(), memoryId(),
         CL_TRUE, offset, size, data, 0, 0, 0);
    context()->reportError("QCLBuffer::read:", error);
    return error == CL_SUCCESS;
}

/*!
    \overload

    Reads \a size bytes from this buffer, starting at offset zero,
    into the supplied \a data array.  Returns true if the read
    was successful; false otherwise.

    This function will block until the request finishes.
    The request is executed on the active command queue for context().

    \sa readAsync(), write()
*/
bool QCLBuffer::read(void *data, size_t size)
{
    cl_int error = clEnqueueReadBuffer
        (context()->activeQueue(), memoryId(),
         CL_TRUE, 0, size, data, 0, 0, 0);
    context()->reportError("QCLBuffer::read:", error);
    return error == CL_SUCCESS;
}

/*!
    Reads \a size bytes from this buffer, starting at \a offset,
    into the supplied \a data array.

    This function will queue the request and return immediately.
    Returns an event object that can be used to wait for the
    request to finish.

    The request will not start until all of the events in \a after
    have been signaled as finished.  The request is executed on
    the active command queue for context().

    \sa read(), writeAsync()
*/
QCLEvent QCLBuffer::readAsync(size_t offset, void *data, size_t size,
                              const QCLEventList &after)
{
    cl_event event;
    cl_int error = clEnqueueReadBuffer
        (context()->activeQueue(), memoryId(), CL_FALSE, offset, size, data,
         after.size(), after.eventData(), &event);
    context()->reportError("QCLBuffer::readAsync:", error);
    if (error != CL_SUCCESS)
        return QCLEvent();
    else
        return QCLEvent(event);
}

/*!
    Writes \a size bytes to this buffer, starting at \a offset,
    from the supplied \a data array.  Returns true if the write
    was successful; false otherwise.

    This function will block until the request finishes.
    The request is executed on the active command queue for context().

    \sa writeAsync(), read()
*/
bool QCLBuffer::write(size_t offset, const void *data, size_t size)
{
    cl_int error = clEnqueueWriteBuffer
        (context()->activeQueue(), memoryId(),
         CL_TRUE, offset, size, data, 0, 0, 0);
    context()->reportError("QCLBuffer::write:", error);
    return error == CL_SUCCESS;
}

/*!
    Writes \a size bytes to this buffer, starting at offset zero,
    from the supplied \a data array.  Returns true if the write
    was successful; false otherwise.

    This function will block until the request finishes.
    The request is executed on the active command queue for context().

    \sa writeAsync(), read()
*/
bool QCLBuffer::write(const void *data, size_t size)
{
    cl_int error = clEnqueueWriteBuffer
        (context()->activeQueue(), memoryId(),
         CL_TRUE, 0, size, data, 0, 0, 0);
    context()->reportError("QCLBuffer::write:", error);
    return error == CL_SUCCESS;
}

/*!
    Writes \a size bytes to this buffer, starting at \a offset,
    from the supplied \a data array.

    This function will queue the request and return immediately.
    Returns an event object that can be used to wait for the
    request to finish.

    The request will not start until all of the events in \a after
    have been signaled as finished.  The request is executed on
    the active command queue for context().

    \sa write(), readAsync()
*/
QCLEvent QCLBuffer::writeAsync(size_t offset, const void *data, size_t size,
                               const QCLEventList &after)
{
    cl_event event;
    cl_int error = clEnqueueWriteBuffer
        (context()->activeQueue(), memoryId(), CL_FALSE, offset, size, data,
         after.size(), after.eventData(), &event);
    context()->reportError("QCLBuffer::writeAsync:", error);
    if (error != CL_SUCCESS)
        return QCLEvent();
    else
        return QCLEvent(event);
}

/*!
    Copies the \a size bytes at \a offset in this buffer
    be copied to \a destOffset in the buffer \a dest.  Returns true
    if the copy was successful; false otherwise.

    This function will block until the request finishes.
    The request is executed on the active command queue for context().

    \sa copyToAsync()
*/
bool QCLBuffer::copyTo
    (size_t offset, size_t size, const QCLBuffer &dest, size_t destOffset)
{
    cl_event event;
    cl_int error = clEnqueueCopyBuffer
        (context()->activeQueue(), memoryId(), dest.memoryId(),
         offset, destOffset, size, 0, 0, &event);
    context()->reportError("QCLBuffer::copyTo(QCLBuffer):", error);
    if (error == CL_SUCCESS) {
        clWaitForEvents(1, &event);
        clReleaseEvent(event);
        return true;
    } else {
        return false;
    }
}

/*!
    Copies the contents of this buffer, starting at \a offset to
    \a rect within \a dest.  Returns true if the copy was successful;
    false otherwise.

    This function will block until the request finishes.
    The request is executed on the active command queue for context().

    \sa copyToAsync()
*/
bool QCLBuffer::copyTo
    (size_t offset, const QCLImage2D &dest, const QRect &rect)
{
    const size_t dst_origin[3] = {rect.x(), rect.y(), 0};
    const size_t region[3] = {rect.width(), rect.height(), 1};
    cl_event event;
    cl_int error = clEnqueueCopyBufferToImage
        (context()->activeQueue(), memoryId(), dest.memoryId(),
         offset, dst_origin, region, 0, 0, &event);
    context()->reportError("QCLBuffer::copyTo(QCLImage2D):", error);
    if (error == CL_SUCCESS) {
        clWaitForEvents(1, &event);
        clReleaseEvent(event);
        return true;
    } else {
        return false;
    }
}

/*!
    Copies the contents of this buffer, starting at \a offset to
    \a origin within \a dest, extending for \a size.  Returns true if
    the copy was successful; false otherwise.

    This function will block until the request finishes.
    The request is executed on the active command queue for context().

    \sa copyToAsync()
*/
bool QCLBuffer::copyTo
    (size_t offset, const QCLImage3D &dest,
     const size_t origin[3], const size_t size[3])
{
    cl_event event;
    cl_int error = clEnqueueCopyBufferToImage
        (context()->activeQueue(), memoryId(), dest.memoryId(),
         offset, origin, size, 0, 0, &event);
    context()->reportError("QCLBuffer::copyTo(QCLImage3D):", error);
    if (error == CL_SUCCESS) {
        clWaitForEvents(1, &event);
        clReleaseEvent(event);
        return true;
    } else {
        return false;
    }
}

/*!
    Requests that the \a size bytes at \a offset in this buffer
    be copied to \a destOffset in the buffer \a dest.  Returns an
    event object that can be used to wait for the request to finish.

    The request will not start until all of the events in \a after
    have been signaled as finished.  The request is executed on
    the active command queue for context().

    \sa copyTo()
*/
QCLEvent QCLBuffer::copyToAsync
    (size_t offset, size_t size, const QCLBuffer &dest, size_t destOffset,
     const QCLEventList &after)
{
    cl_event event;
    cl_int error = clEnqueueCopyBuffer
        (context()->activeQueue(), memoryId(), dest.memoryId(),
         offset, destOffset, size,
         after.size(), after.eventData(), &event);
    context()->reportError("QCLBuffer::copyToAsync:", error);
    if (error != CL_SUCCESS)
        return QCLEvent();
    else
        return QCLEvent(event);
}

/*!
    Copies the contents of this buffer, starting at \a offset to
    \a rect within \a dest.  Returns an event object that can be used
    to wait for the request to finish.

    The request will not start until all of the events in \a after
    have been signaled as finished.  The request is executed on
    the active command queue for context().

    \sa copyTo()
*/
QCLEvent QCLBuffer::copyToAsync
    (size_t offset, const QCLImage2D &dest, const QRect &rect,
     const QCLEventList &after)
{
    const size_t dst_origin[3] = {rect.x(), rect.y(), 0};
    const size_t region[3] = {rect.width(), rect.height(), 1};
    cl_event event;
    cl_int error = clEnqueueCopyBufferToImage
        (context()->activeQueue(), memoryId(), dest.memoryId(),
         offset, dst_origin, region,
         after.size(), after.eventData(), &event);
    context()->reportError("QCLBuffer::copyToAsync(QCLImage2D):", error);
    if (error == CL_SUCCESS)
        return QCLEvent(event);
    else
        return QCLEvent();
}

/*!
    Copies the contents of this buffer, starting at \a offset to
    \a origin within \a dest, extending for \a size.  Returns an event
    object that can be used to wait for the request to finish.

    The request will not start until all of the events in \a after
    have been signaled as finished.  The request is executed on
    the active command queue for context().

    \sa copyTo()
*/
QCLEvent QCLBuffer::copyToAsync
    (size_t offset, const QCLImage3D &dest,
     const size_t origin[3], const size_t size[3],
     const QCLEventList &after)
{
    cl_event event;
    cl_int error = clEnqueueCopyBufferToImage
        (context()->activeQueue(), memoryId(), dest.memoryId(),
         offset, origin, size,
         after.size(), after.eventData(), &event);
    context()->reportError("QCLBuffer::copyToAsync(QCLImage3D):", error);
    if (error == CL_SUCCESS)
        return QCLEvent(event);
    else
        return QCLEvent();
}

// We use a single enum in the public API for both memory objects
// and memory mapping, but the values are slightly different in
// the OpenCL C API.  This function corrects for the difference.
cl_map_flags qt_cl_map_flags(QCLMemoryObject::Access access)
{
    if (access == QCLMemoryObject::ReadOnly)
        return CL_MAP_READ;
    else if (access == QCLMemoryObject::WriteOnly)
        return CL_MAP_WRITE;
    else
        return CL_MAP_READ | CL_MAP_WRITE;
}

/*!
    Maps the \a size bytes starting at \a offset in this buffer
    into host memory for the specified \a access mode.  Returns a
    pointer to the mapped region.

    This function will block until the request finishes.
    The request is executed on the active command queue for context().

    \sa mapAsync(), unmap()
*/
void *QCLBuffer::map
    (size_t offset, size_t size, QCLMemoryObject::Access access)
{
    cl_int error;
    void *data = clEnqueueMapBuffer
        (context()->activeQueue(), memoryId(), CL_TRUE,
         qt_cl_map_flags(access), offset, size, 0, 0, 0, &error);
    context()->reportError("QCLBuffer::map:", error);
    return data;
}

/*!
    \overload

    Maps the entire contents of this buffer into host memory for
    the specified \a access mode.  Returns a pointer to the
    mapped region.
*/
void *QCLBuffer::map(QCLMemoryObject::Access access)
{
    return map(0, size(), access);
}

/*!
    Maps the \a size bytes starting at \a offset in this buffer
    into host memory for the specified \a access mode.  Returns a
    pointer to the mapped region in \a ptr, which will be valid
    only after the request finishes.

    This function will queue the request and return immediately.
    Returns an event object that can be used to wait for the
    request to finish.

    The request will not start until all of the events in \a after
    have been signaled as finished.  The request is executed on
    the active command queue for context().

    \sa map(), unmapAsync()
*/
QCLEvent QCLBuffer::mapAsync
    (void **ptr, size_t offset, size_t size,
     QCLMemoryObject::Access access, const QCLEventList &after)
{
    cl_int error;
    cl_event event;
    *ptr = clEnqueueMapBuffer
        (context()->activeQueue(), memoryId(), CL_FALSE,
         qt_cl_map_flags(access), offset, size,
         after.size(), after.eventData(), &event, &error);
    context()->reportError("QCLBuffer::mapAsync:", error);
    if (error == CL_SUCCESS)
        return QCLEvent(event);
    else
        return QCLEvent();
}

QT_END_NAMESPACE
