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

#include "qclimage.h"
#include "qclbuffer.h"
#include "qclcontext.h"
#include "qcl_gl_p.h"

QT_BEGIN_NAMESPACE

/*!
    \class QCLImage2D
    \brief The QCLImage2D class represents a 2D OpenCL image object.
    \since 4.7
    \ingroup opencl
*/

/*!
    \fn QCLImage2D::QCLImage2D()

    Constructs a null 2D OpenCL image object.
*/

/*!
    \fn QCLImage2D::QCLImage2D(QCLContext *context, cl_mem id)

    Constructs a 2D OpenCL image object that is initialized with the
    native OpenCL identifier \a id, and associates it with \a context.
    This class will take over ownership of \a id and will release
    it in the destructor.
*/

/*!
    \fn QCLImage2D::QCLImage2D(const QCLImage2D& other)

    Constructs a copy of \a other.
*/

/*!
    \fn QCLImage2D& QCLImage2D::operator=(const QCLImage2D& other)

    Assigns \a other to this object.
*/

/*!
    Returns the format descriptor for this OpenCL image.
*/
QCLImageFormat QCLImage2D::format() const
{
    cl_image_format iformat;
    if (clGetImageInfo(id(), CL_IMAGE_FORMAT, sizeof(iformat), &iformat, 0)
            != CL_SUCCESS)
        return QCLImageFormat();
    else
        return QCLImageFormat
            (QCLImageFormat::ChannelOrder(iformat.image_channel_order),
             QCLImageFormat::ChannelType(iformat.image_channel_data_type));
}

static int qt_cl_imageParam(cl_mem id, cl_image_info name)
{
    size_t value = 0;
    if (clGetImageInfo(id, name, sizeof(value), &value, 0) != CL_SUCCESS)
        return 0;
    else
        return int(value);
}

/*!
    Returns the width of this OpenCL image.

    \sa height(), bytesPerLine()
*/
int QCLImage2D::width() const
{
    return qt_cl_imageParam(id(), CL_IMAGE_WIDTH);
}

/*!
    Returns the height of this OpenCL image.

    \sa width()
*/
int QCLImage2D::height() const
{
    return qt_cl_imageParam(id(), CL_IMAGE_HEIGHT);
}

/*!
    Returns the number of bytes per element in this OpenCL image.

    \sa bytesPerLine()
*/
int QCLImage2D::bytesPerElement() const
{
    return qt_cl_imageParam(id(), CL_IMAGE_ELEMENT_SIZE);
}

/*!
    Returns the number of bytes per line in this OpenCL image.

    \sa bytesPerElement()
*/
int QCLImage2D::bytesPerLine() const
{
    return qt_cl_imageParam(id(), CL_IMAGE_ROW_PITCH);
}

/*!
    Returns true if this 2D OpenCL image object is also an OpenGL
    2D texture object; false otherwise.

    \sa isRenderbuffer(), QCLImage3D::isTexture3D()
*/
bool QCLImage2D::isTexture2D() const
{
#ifndef QT_NO_CL_OPENGL
    cl_gl_object_type objectType;
    if (clGetGLObjectInfo(id(), &objectType, 0) != CL_SUCCESS)
        return false;
    return objectType == CL_GL_OBJECT_TEXTURE2D;
#else
    return false;
#endif
}

/*!
    Returns true if this 2D OpenCL image object is also an OpenGL
    renderbuffer object; false otherwise.

    \sa isTexture2D()
*/
bool QCLImage2D::isRenderbuffer() const
{
#ifndef QT_NO_CL_OPENGL
    cl_gl_object_type objectType;
    if (clGetGLObjectInfo(id(), &objectType, 0) != CL_SUCCESS)
        return false;
    return objectType == CL_GL_OBJECT_RENDERBUFFER;
#else
    return false;
#endif
}

/*!
    Reads the contents of \a rect from within this image into \a data.
    Returns true if the read was successful; false otherwise.
    If \a bytesPerLine is not zero, it indicates the number of bytes
    between lines in \a data.

    This function will block until the request completes.
    The request is executed on the active command queue for context().

    \sa readAsync(), write(), toQImage()
*/
bool QCLImage2D::read(void *data, const QRect& rect, int bytesPerLine)
{
    size_t origin[3] = {rect.x(), rect.y(), 0};
    size_t region[3] = {rect.width(), rect.height(), 1};
    cl_int error = clEnqueueReadImage
        (context()->activeQueue(), id(), CL_TRUE,
         origin, region, bytesPerLine, 0, data, 0, 0, 0);
    context()->reportError("QCLImage2D::read:", error);
    return error == CL_SUCCESS;
}

/*!
    \overload

    Reads the contents of \a rect from within this 2D OpenCL image
    into \a image.  Returns true if the read was successful; false otherwise.
    If \a rect is null, then the entire image is read.

    This function will block until the request completes.
    The request is executed on the active command queue for context().
*/
bool QCLImage2D::read(QImage *image, const QRect& rect)
{
    if (rect.isNull()) {
        return read(image->bits(),
                    QRect(0, 0, image->width(), image->height()),
                    image->bytesPerLine());
    } else {
        return read(image->bits(), rect, image->bytesPerLine());
    }
}

/*!
    Reads the contents of \a rect from within this image into \a data.
    Returns true if the read was successful; false otherwise.
    If \a bytesPerLine is not zero, it indicates the number of bytes
    between lines in \a data.

    This function will queue the request and return immediately.
    Returns an event object that can be used to wait for the
    request to complete.

    The request will not start until all of the events in \a after
    have been signalled as completed.  The request is executed on
    the active command queue for context().

    \sa read(), writeAsync()
*/
QCLEvent QCLImage2D::readAsync
    (void *data, const QRect& rect,
     const QVector<QCLEvent>& after, int bytesPerLine)
{
    size_t origin[3] = {rect.x(), rect.y(), 0};
    size_t region[3] = {rect.width(), rect.height(), 1};
    cl_event event;
    cl_int error = clEnqueueReadImage
        (context()->activeQueue(), id(), CL_FALSE,
         origin, region, bytesPerLine, 0, data, after.size(),
         (after.isEmpty() ? 0 :
            reinterpret_cast<const cl_event *>(after.constData())), &event);
    context()->reportError("QCLImage2D::readAsync:", error);
    if (error == CL_SUCCESS)
        return QCLEvent(event);
    else
        return QCLEvent();
}

/*!
    Writes the contents \a data to \a rect within this image.
    Returns true if the write was successful; false otherwise.
    If \a bytesPerLine is not zero, it indicates the number of bytes
    between lines in \a data.

    This function will block until the request completes.
    The request is executed on the active command queue for context().

    \sa writeAsync(), read()
*/
bool QCLImage2D::write(const void *data, const QRect& rect, int bytesPerLine)
{
    size_t origin[3] = {rect.x(), rect.y(), 0};
    size_t region[3] = {rect.width(), rect.height(), 1};
    cl_int error = clEnqueueWriteImage
        (context()->activeQueue(), id(), CL_TRUE,
         origin, region, bytesPerLine, 0, data, 0, 0, 0);
    context()->reportError("QCLImage2D::write:", error);
    return error == CL_SUCCESS;
}

/*!
    \overload

    Writes the contents of \a image to \a rect within this 2D OpenCL image.
    Returns true if the write was successful; false otherwise.
    If \a rect is null, then the entire image is read.

    This function will block until the request completes.
    The request is executed on the active command queue for context().
*/
bool QCLImage2D::write(const QImage& image, const QRect& rect)
{
    if (rect.isNull()) {
        return write(image.bits(),
                     QRect(0, 0, image.width(), image.height()),
                     image.bytesPerLine());
    } else {
        return write(image.bits(), rect, image.bytesPerLine());
    }
}

/*!
    Writes the contents of \a data into \a rect within this image.
    Returns true if the write was successful; false otherwise.
    If \a bytesPerLine is not zero, it indicates the number of bytes
    between lines in \a data.

    This function will queue the request and return immediately.
    Returns an event object that can be used to wait for the
    request to complete.

    The request will not start until all of the events in \a after
    have been signalled as completed.  The request is executed on
    the active command queue for context().

    \sa write(), readAsync()
*/
QCLEvent QCLImage2D::writeAsync
    (const void *data, const QRect& rect,
     const QVector<QCLEvent>& after, int bytesPerLine)
{
    size_t origin[3] = {rect.x(), rect.y(), 0};
    size_t region[3] = {rect.width(), rect.height(), 1};
    cl_event event;
    cl_int error = clEnqueueWriteImage
        (context()->activeQueue(), id(), CL_FALSE,
         origin, region, bytesPerLine, 0, data, after.size(),
         (after.isEmpty() ? 0 :
            reinterpret_cast<const cl_event *>(after.constData())), &event);
    context()->reportError("QCLImage2D::writeAsync:", error);
    if (error == CL_SUCCESS)
        return QCLEvent(event);
    else
        return QCLEvent();
}

/*!
    Copies the contents of \a rect from this image to \a destOffset
    in \a dest.  Returns true if the copy was successful; false otherwise.

    This function will block until the request completes.
    The request is executed on the active command queue for context().

    \sa copyToAsync()
*/
bool QCLImage2D::copyTo
    (const QRect& rect, const QCLImage2D& dest, const QPoint& destOffset)
{
    size_t src_origin[3] = {rect.x(), rect.y(), 0};
    size_t dst_origin[3] = {destOffset.x(), destOffset.y(), 0};
    size_t region[3] = {rect.width(), rect.height(), 1};
    cl_event event;
    cl_int error = clEnqueueCopyImage
        (context()->activeQueue(), id(), dest.id(),
         src_origin, dst_origin, region, 0, 0, &event);
    context()->reportError("QCLImage2D::copyTo(QCLImage2D):", error);
    if (error == CL_SUCCESS) {
        clWaitForEvents(1, &event);
        clReleaseEvent(event);
        return true;
    } else {
        return false;
    }
}

/*!
    Copies the contents of \a rect from this image to \a destOffset
    in \a dest.  Returns true if the copy was successful; false otherwise.

    This function will block until the request completes.
    The request is executed on the active command queue for context().

    \sa copyToAsync()
*/
bool QCLImage2D::copyTo
    (const QRect& rect, const QCLImage3D& dest, const size_t destOffset[3])
{
    size_t src_origin[3] = {rect.x(), rect.y(), 0};
    size_t region[3] = {rect.width(), rect.height(), 1};
    cl_event event;
    cl_int error = clEnqueueCopyImage
        (context()->activeQueue(), id(), dest.id(),
         src_origin, destOffset, region, 0, 0, &event);
    context()->reportError("QCLImage2D::copyTo(QCLImage3D):", error);
    if (error == CL_SUCCESS) {
        clWaitForEvents(1, &event);
        clReleaseEvent(event);
        return true;
    } else {
        return false;
    }
}

/*!
    Copies the contents of \a rect from this image to \a destOffset
    in \a dest.  Returns true if the copy was successful; false otherwise.

    This function will block until the request completes.
    The request is executed on the active command queue for context().

    \sa copyToAsync()
*/
bool QCLImage2D::copyTo
    (const QRect& rect, const QCLBuffer& dest, size_t destOffset)
{
    size_t src_origin[3] = {rect.x(), rect.y(), 0};
    size_t region[3] = {rect.width(), rect.height(), 1};
    cl_event event;
    cl_int error = clEnqueueCopyImageToBuffer
        (context()->activeQueue(), id(), dest.id(),
         src_origin, region, destOffset, 0, 0, &event);
    context()->reportError("QCLImage2D::copyTo(QCLBuffer):", error);
    if (error == CL_SUCCESS) {
        clWaitForEvents(1, &event);
        clReleaseEvent(event);
        return true;
    } else {
        return false;
    }
}

/*!
    Requests that the contents of \a rect from this image be copied
    to \a destOffset in \a dest.  Returns an event object that can be
    used to wait for the request to complete.

    The request will not start until all of the events in \a after
    have been signalled as completed.  The request is executed on
    the active command queue for context().

    \sa copyTo()
*/
QCLEvent QCLImage2D::copyToAsync
    (const QRect& rect, const QCLImage2D& dest, const QPoint& destOffset,
     const QVector<QCLEvent>& after)
{
    size_t src_origin[3] = {rect.x(), rect.y(), 0};
    size_t dst_origin[3] = {destOffset.x(), destOffset.y(), 0};
    size_t region[3] = {rect.width(), rect.height(), 1};
    cl_event event;
    cl_int error = clEnqueueCopyImage
        (context()->activeQueue(), id(), dest.id(),
         src_origin, dst_origin, region, after.size(),
         (after.isEmpty() ? 0 :
            reinterpret_cast<const cl_event *>(after.constData())), &event);
    context()->reportError("QCLImage2D::copyToAsync(QCLImage2D):", error);
    if (error == CL_SUCCESS)
        return QCLEvent(event);
    else
        return QCLEvent();
}

/*!
    Requests that the contents of \a rect from this image be copied
    to \a destOffset in \a dest.  Returns an event object that can be
    used to wait for the request to complete.

    The request will not start until all of the events in \a after
    have been signalled as completed.  The request is executed on
    the active command queue for context().

    \sa copyTo()
*/
QCLEvent QCLImage2D::copyToAsync
    (const QRect& rect, const QCLImage3D& dest, const size_t destOffset[3],
     const QVector<QCLEvent>& after)
{
    size_t src_origin[3] = {rect.x(), rect.y(), 0};
    size_t region[3] = {rect.width(), rect.height(), 1};
    cl_event event;
    cl_int error = clEnqueueCopyImage
        (context()->activeQueue(), id(), dest.id(),
         src_origin, destOffset, region, after.size(),
         (after.isEmpty() ? 0 :
            reinterpret_cast<const cl_event *>(after.constData())), &event);
    context()->reportError("QCLImage2D::copyToAsync(QCLImage3D):", error);
    if (error == CL_SUCCESS)
        return QCLEvent(event);
    else
        return QCLEvent();
}

/*!
    Requests that the contents of \a rect from this image be copied
    to \a destOffset in \a dest.  Returns an event object that can be
    used to wait for the request to complete.

    The request will not start until all of the events in \a after
    have been signalled as completed.  The request is executed on
    the active command queue for context().

    \sa copyTo()
*/
QCLEvent QCLImage2D::copyToAsync
    (const QRect& rect, const QCLBuffer& dest, size_t destOffset,
     const QVector<QCLEvent>& after)
{
    size_t src_origin[3] = {rect.x(), rect.y(), 0};
    size_t region[3] = {rect.width(), rect.height(), 1};
    cl_event event;
    cl_int error = clEnqueueCopyImageToBuffer
        (context()->activeQueue(), id(), dest.id(),
         src_origin, region, destOffset, after.size(),
         (after.isEmpty() ? 0 :
            reinterpret_cast<const cl_event *>(after.constData())), &event);
    context()->reportError("QCLImage2D::copyToAsync(QCLBuffer):", error);
    if (error == CL_SUCCESS)
        return QCLEvent(event);
    else
        return QCLEvent();
}

// Defined in qclbuffer.cpp.
extern cl_map_flags qt_cl_map_flags(QCLMemoryObject::Access access);

/*!
    Maps the image region \a rect into host memory for the
    specified \a access mode.  Returns a pointer to the mapped region.

    If \a bytesPerLine is not null, it will return the number of
    bytes per line in the mapped image.

    This function will block until the request completes.
    The request is executed on the active command queue for context().

    \sa mapAsync(), unmap()
*/
void *QCLImage2D::map
    (const QRect& rect, QCLMemoryObject::Access access, int *bytesPerLine)
{
    size_t origin[3] = {rect.x(), rect.y(), 0};
    size_t region[3] = {rect.width(), rect.height(), 1};
    cl_int error;
    size_t rowPitch;
    void *data = clEnqueueMapImage
        (context()->activeQueue(), id(), CL_TRUE,
         qt_cl_map_flags(access), origin, region,
         &rowPitch, 0, 0, 0, 0, &error);
    context()->reportError("QCLImage2D::map:", error);
    if (bytesPerLine)
        *bytesPerLine = int(rowPitch);
    return data;
}

/*!
    Maps the image region \a rect into host memory for the specified
    \a access mode.  Returns a pointer to the mapped region in \a ptr,
    which will be valid only after the request completes.

    If \a bytesPerLine is not null, it will return the number of
    bytes per line in the mapped image.

    This function will queue the request and return immediately.
    Returns an event object that can be used to wait for the
    request to complete.

    The request will not start until all of the events in \a after
    have been signalled as completed.  The request is executed on
    the active command queue for context().

    \sa map(), unmapAsync()
*/
QCLEvent QCLImage2D::mapAsync
    (void **ptr, const QRect& rect, QCLMemoryObject::Access access,
     const QVector<QCLEvent>& after, int *bytesPerLine)
{
    size_t origin[3] = {rect.x(), rect.y(), 0};
    size_t region[3] = {rect.width(), rect.height(), 1};
    cl_int error;
    size_t rowPitch;
    cl_event event;
    *ptr = clEnqueueMapImage
        (context()->activeQueue(), id(), CL_FALSE,
         qt_cl_map_flags(access), origin, region, &rowPitch, 0, after.size(),
         (after.isEmpty() ? 0 :
            reinterpret_cast<const cl_event *>(after.constData())),
         &event, &error);
    context()->reportError("QCLImage2D::mapAsync:", error);
    if (bytesPerLine)
        *bytesPerLine = int(rowPitch);
    if (error == CL_SUCCESS)
        return QCLEvent(event);
    else
        return QCLEvent();
}

/*!
    Reads the contents of this 2D OpenCL image and returns it
    as a QImage.  Returns a null QImage if the OpenCL image's
    format cannot be converted into a QImage format.

    \sa read()
*/
QImage QCLImage2D::toQImage()
{
    if (!id())
        return QImage();
    QImage::Format qformat = format().toQImageFormat();
    if (qformat == QImage::Format_Invalid)
        return QImage();
    QImage image(width(), height(), qformat);
    if (!read(image.bits(), QRect(0, 0, image.width(), image.height()),
              image.bytesPerLine()))
        return QImage();
    return image;
}

/*!
    \class QCLImage3D
    \brief The QCLImage3D class represents a 3D OpenCL image object.
    \since 4.7
    \ingroup opencl
*/

/*!
    \fn QCLImage3D::QCLImage3D()

    Constructs a null 3D OpenCL image object.
*/

/*!
    \fn QCLImage3D::QCLImage3D(QCLContext *context, cl_mem id)

    Constructs a 3D OpenCL image object that is initialized with the
    native OpenCL identifier \a id, and associates it with \a context.
    This class will take over ownership of \a id and will release
    it in the destructor.
*/

/*!
    \fn QCLImage3D::QCLImage3D(const QCLImage3D& other)

    Constructs a copy of \a other.
*/

/*!
    \fn QCLImage3D& QCLImage3D::operator=(const QCLImage3D& other)

    Assigns \a other to this object.
*/

/*!
    Returns the format descriptor for this OpenCL image.
*/
QCLImageFormat QCLImage3D::format() const
{
    cl_image_format iformat;
    if (clGetImageInfo(id(), CL_IMAGE_FORMAT, sizeof(iformat), &iformat, 0)
            != CL_SUCCESS)
        return QCLImageFormat();
    else
        return QCLImageFormat
            (QCLImageFormat::ChannelOrder(iformat.image_channel_order),
             QCLImageFormat::ChannelType(iformat.image_channel_data_type));
}

/*!
    Returns the width of this OpenCL image.

    \sa height(), depth(), bytesPerLine()
*/
int QCLImage3D::width() const
{
    return qt_cl_imageParam(id(), CL_IMAGE_WIDTH);
}

/*!
    Returns the height of this OpenCL image.

    \sa width(), depth()
*/
int QCLImage3D::height() const
{
    return qt_cl_imageParam(id(), CL_IMAGE_HEIGHT);
}

/*!
    Returns the depth of this 3D OpenCL image; 0 if the image is 2D.

    \sa width(), height(), bytesPerSlice()
*/
int QCLImage3D::depth() const
{
    return qt_cl_imageParam(id(), CL_IMAGE_DEPTH);
}

/*!
    Returns the number of bytes per element in this OpenCL image.

    \sa bytesPerLine(), bytesPerSlice()
*/
int QCLImage3D::bytesPerElement() const
{
    return qt_cl_imageParam(id(), CL_IMAGE_ELEMENT_SIZE);
}

/*!
    Returns the number of bytes per line in this OpenCL image.

    \sa bytesPerElement(), bytesPerSlice()
*/
int QCLImage3D::bytesPerLine() const
{
    return qt_cl_imageParam(id(), CL_IMAGE_ROW_PITCH);
}

/*!
    Returns the number of bytes per 2D slice in this 3D OpenCL image;
    0 if the image is 2D.

    \sa bytesPerElement(), bytesPerLine()
*/
int QCLImage3D::bytesPerSlice() const
{
    return qt_cl_imageParam(id(), CL_IMAGE_SLICE_PITCH);
}

/*!
    Returns true if this 3D OpenCL image object is also an OpenGL
    3D texture object; false otherwise.

    \sa QCLImage2D::isTexture2D()
*/
bool QCLImage3D::isTexture3D() const
{
#ifndef QT_NO_CL_OPENGL
    cl_gl_object_type objectType;
    if (clGetGLObjectInfo(id(), &objectType, 0) != CL_SUCCESS)
        return false;
    return objectType == CL_GL_OBJECT_TEXTURE3D;
#else
    return false;
#endif
}

/*!
    Reads the contents of this 3D image, starting at \a origin,
    and extending for \a size, into \a data.  Returns true if the read
    was successful; false otherwise.  If \a bytesPerLine is not zero,
    it indicates the number of bytes between lines in \a data.
    If \a bytesPerSlice is not zero, it indicates the number of bytes
    between slices in \a data.

    This function will block until the request completes.
    The request is executed on the active command queue for context().

    \sa readAsync(), write()
*/
bool QCLImage3D::read
    (void *data, const size_t origin[3], const size_t size[3],
     int bytesPerLine, int bytesPerSlice)
{
    cl_int error = clEnqueueReadImage
        (context()->activeQueue(), id(), CL_TRUE,
         origin, size, bytesPerLine, bytesPerSlice, data, 0, 0, 0);
    context()->reportError("QCLImage3D::read:", error);
    return error == CL_SUCCESS;
}

/*!
    Reads the contents of this 3D image, starting at \a origin,
    and extending for \a size, into \a data.  Returns true if the read
    was successful; false otherwise.  If \a bytesPerLine is not zero,
    it indicates the number of bytes between lines in \a data.
    If \a bytesPerSlice is not zero, it indicates the number of bytes
    between slices in \a data.

    This function will queue the request and return immediately.
    Returns an event object that can be used to wait for the
    request to complete.

    The request will not start until all of the events in \a after
    have been signalled as completed.  The request is executed on
    the active command queue for context().

    \sa read(), writeAsync()
*/
QCLEvent QCLImage3D::readAsync
    (void *data, const size_t origin[3], const size_t size[3],
     const QVector<QCLEvent> after, int bytesPerLine, int bytesPerSlice)
{
    cl_event event;
    cl_int error = clEnqueueReadImage
        (context()->activeQueue(), id(), CL_FALSE,
         origin, size, bytesPerLine, bytesPerSlice, data, after.size(),
         (after.isEmpty() ? 0 :
            reinterpret_cast<const cl_event *>(after.constData())), &event);
    context()->reportError("QCLImage3D::readAsync:", error);
    if (error == CL_SUCCESS)
        return QCLEvent(event);
    else
        return QCLEvent();
}

/*!
    Writes the contents of this 3D image, starting at \a origin,
    and extending for \a size, to \a data.  Returns true if the write
    was successful; false otherwise.  If \a bytesPerLine is not zero,
    it indicates the number of bytes between lines in \a data.
    If \a bytesPerSlice is not zero, it indicates the number of bytes
    between slices in \a data.

    This function will block until the request completes.
    The request is executed on the active command queue for context().

    \sa read(), writeAsync()
*/
bool QCLImage3D::write
    (const void *data, const size_t origin[3], const size_t size[3],
     int bytesPerLine, int bytesPerSlice)
{
    cl_int error = clEnqueueWriteImage
        (context()->activeQueue(), id(), CL_TRUE,
         origin, size, bytesPerLine, bytesPerSlice, data, 0, 0, 0);
    context()->reportError("QCLImage3D::write:", error);
    return error == CL_SUCCESS;
}

/*!
    Writes the contents of this 3D image, starting at \a origin,
    and extending for \a size, to \a data.  Returns true if the write
    was successful; false otherwise.  If \a bytesPerLine is not zero,
    it indicates the number of bytes between lines in \a data.
    If \a bytesPerSlice is not zero, it indicates the number of bytes
    between slices in \a data.

    This function will queue the request and return immediately.
    Returns an event object that can be used to wait for the
    request to complete.

    The request will not start until all of the events in \a after
    have been signalled as completed.  The request is executed on
    the active command queue for context().

    \sa readAsync(), write()
*/
QCLEvent QCLImage3D::writeAsync
    (const void *data, const size_t origin[3], const size_t size[3],
     const QVector<QCLEvent> after, int bytesPerLine, int bytesPerSlice)
{
    cl_event event;
    cl_int error = clEnqueueWriteImage
        (context()->activeQueue(), id(), CL_FALSE,
         origin, size, bytesPerLine, bytesPerSlice, data, after.size(),
         (after.isEmpty() ? 0 :
            reinterpret_cast<const cl_event *>(after.constData())), &event);
    context()->reportError("QCLImage3D::writeAsync:", error);
    if (error == CL_SUCCESS)
        return QCLEvent(event);
    else
        return QCLEvent();
}

/*!
    Copies the contents of this 3D image, starting at \a origin, and
    extending for \a size, to \a destOffset in \a dest.  Returns true
    if the copy was successful; false otherwise.

    This function will block until the request completes.
    The request is executed on the active command queue for context().

    \sa copyToAsync()
*/
bool QCLImage3D::copyTo
    (const size_t origin[3], const size_t size[3],
     const QCLImage3D& dest, const size_t destOffset[3])
{
    cl_event event;
    cl_int error = clEnqueueCopyImage
        (context()->activeQueue(), id(), dest.id(),
         origin, destOffset, size, 0, 0, &event);
    context()->reportError("QCLImage3D::copyTo(QCLImage3D):", error);
    if (error == CL_SUCCESS) {
        clWaitForEvents(1, &event);
        clReleaseEvent(event);
        return true;
    } else {
        return false;
    }
}

/*!
    Copies the contents of a single slice within this 3D image,
    starting at \a origin, and extending for \a size,
    to \a destOffset in \a dest.  Returns true if the copy was
    successful; false otherwise.

    This function will block until the request completes.
    The request is executed on the active command queue for context().

    \sa copyToAsync()
*/
bool QCLImage3D::copyTo
    (const size_t origin[3], const QSize& size, const QCLImage2D& dest,
     const QPoint& destOffset)
{
    size_t dst_origin[3] = {destOffset.x(), destOffset.y(), 0};
    size_t region[3] = {size.width(), size.height(), 1};
    cl_event event;
    cl_int error = clEnqueueCopyImage
        (context()->activeQueue(), id(), dest.id(),
         origin, dst_origin, region, 0, 0, &event);
    context()->reportError("QCLImage3D::copyTo(QCLImage2D):", error);
    if (error == CL_SUCCESS) {
        clWaitForEvents(1, &event);
        clReleaseEvent(event);
        return true;
    } else {
        return false;
    }
}

/*!
    Copies the contents of this 3D image, starting at \a origin, and
    extending for \a size, to \a destOffset in \a dest.  Returns true
    if the copy was successful; false otherwise.

    This function will block until the request completes.
    The request is executed on the active command queue for context().

    \sa copyToAsync()
*/
bool QCLImage3D::copyTo
    (const size_t origin[3], const size_t size[3],
     const QCLBuffer& dest, size_t destOffset)
{
    cl_event event;
    cl_int error = clEnqueueCopyImageToBuffer
        (context()->activeQueue(), id(), dest.id(),
         origin, size, destOffset, 0, 0, &event);
    context()->reportError("QCLImage3D::copyTo(QCLBuffer):", error);
    if (error == CL_SUCCESS) {
        clWaitForEvents(1, &event);
        clReleaseEvent(event);
        return true;
    } else {
        return false;
    }
}

/*!
    Copies the contents of this 3D image, starting at \a origin, and
    extending for \a size, to \a destOffset in \a dest.  Returns true
    if the copy was successful; false otherwise.

    This function will queue the request and return immediately.
    Returns an event object that can be used to wait for the
    request to complete.

    The request will not start until all of the events in \a after
    have been signalled as completed.  The request is executed on
    the active command queue for context().

    \sa copyTo()
*/
QCLEvent QCLImage3D::copyToAsync
    (const size_t origin[3], const size_t size[3],
     const QCLImage3D& dest, const size_t destOffset[3],
     const QVector<QCLEvent>& after)
{
    cl_event event;
    cl_int error = clEnqueueCopyImage
        (context()->activeQueue(), id(), dest.id(),
         origin, destOffset, size, after.size(),
         (after.isEmpty() ? 0 :
            reinterpret_cast<const cl_event *>(after.constData())), &event);
    context()->reportError("QCLImage3D::copyToAsync(QCLImage3D):", error);
    if (error == CL_SUCCESS)
        return QCLEvent(event);
    else
        return QCLEvent();
}

/*!
    Copies the contents of a single slice within this 3D image,
    starting at \a origin, and extending for \a size,
    to \a destOffset in \a dest.  Returns true if the copy was
    successful; false otherwise.

    This function will queue the request and return immediately.
    Returns an event object that can be used to wait for the
    request to complete.

    The request will not start until all of the events in \a after
    have been signalled as completed.  The request is executed on
    the active command queue for context().

    \sa copyTo()
*/
QCLEvent QCLImage3D::copyToAsync
    (const size_t origin[3], const QSize& size,
     const QCLImage2D& dest, const QPoint& destOffset,
     const QVector<QCLEvent>& after)
{
    size_t dst_origin[3] = {destOffset.x(), destOffset.y(), 0};
    size_t region[3] = {size.width(), size.height(), 1};
    cl_event event;
    cl_int error = clEnqueueCopyImage
        (context()->activeQueue(), id(), dest.id(),
         origin, dst_origin, region, after.size(),
         (after.isEmpty() ? 0 :
            reinterpret_cast<const cl_event *>(after.constData())), &event);
    context()->reportError("QCLImage3D::copyToAsync(QCLImage2D):", error);
    if (error == CL_SUCCESS)
        return QCLEvent(event);
    else
        return QCLEvent();
}

/*!
    Copies the contents of this 3D image, starting at \a origin, and
    extending for \a size, to \a destOffset in \a dest.  Returns true
    if the copy was successful; false otherwise.

    This function will queue the request and return immediately.
    Returns an event object that can be used to wait for the
    request to complete.

    The request will not start until all of the events in \a after
    have been signalled as completed.  The request is executed on
    the active command queue for context().

    \sa copyTo()
*/
QCLEvent QCLImage3D::copyToAsync
    (const size_t origin[3], const size_t size[3],
     const QCLBuffer& dest, size_t destOffset,
     const QVector<QCLEvent>& after)
{
    cl_event event;
    cl_int error = clEnqueueCopyImageToBuffer
        (context()->activeQueue(), id(), dest.id(),
         origin, size, destOffset, after.size(),
         (after.isEmpty() ? 0 :
            reinterpret_cast<const cl_event *>(after.constData())), &event);
    context()->reportError("QCLImage3D::copyToAsync(QCLBuffer):", error);
    if (error == CL_SUCCESS)
        return QCLEvent(event);
    else
        return QCLEvent();
}

/*!
    Maps the image region starting at \a origin and extending for
    \a size into host memory for the specified \a access mode.
    Returns a pointer to the mapped region.

    If \a bytesPerLine is not null, it will return the number of
    bytes per line in the mapped image.  If \a bytesPerSlice is not null,
    it will return the number of bytes per slice in the mapped image.

    This function will block until the request completes.
    The request is executed on the active command queue for context().

    \sa mapAsync(), unmap()
*/
void *QCLImage3D::map
    (const size_t origin[3], const size_t size[3],
     QCLMemoryObject::Access access, int *bytesPerLine, int *bytesPerSlice)
{
    cl_int error;
    size_t rowPitch, slicePitch;
    void *data = clEnqueueMapImage
        (context()->activeQueue(), id(), CL_TRUE,
         qt_cl_map_flags(access), origin, size,
         &rowPitch, &slicePitch, 0, 0, 0, &error);
    context()->reportError("QCLImage3D::map:", error);
    if (bytesPerLine)
        *bytesPerLine = int(rowPitch);
    if (bytesPerSlice)
        *bytesPerSlice = int(slicePitch);
    return data;
}

/*!
    Maps the image region starting at \a origin and extending for
    \a size into host memory for the specified \a access mode.
    Returns a pointer to the mapped region in \a ptr, which will be
    valid only after the request completes.

    If \a bytesPerLine is not null, it will return the number of
    bytes per line in the mapped image.  If \a bytesPerSlice is not null,
    it will return the number of bytes per slice in the mapped image.

    This function will queue the request and return immediately.
    Returns an event object that can be used to wait for the
    request to complete.

    The request will not start until all of the events in \a after
    have been signalled as completed.  The request is executed on
    the active command queue for context().

    \sa map(), unmap()
*/
QCLEvent QCLImage3D::mapAsync
    (void **ptr, const size_t origin[3], const size_t size[3],
     QCLMemoryObject::Access access, const QVector<QCLEvent>& after,
     int *bytesPerLine, int *bytesPerSlice)
{
    cl_int error;
    size_t rowPitch, slicePitch;
    cl_event event;
    *ptr = clEnqueueMapImage
        (context()->activeQueue(), id(), CL_FALSE, qt_cl_map_flags(access),
         origin, size, &rowPitch, &slicePitch, after.size(),
         (after.isEmpty() ? 0 :
            reinterpret_cast<const cl_event *>(after.constData())),
         &event, &error);
    context()->reportError("QCLImage3D::mapAsync:", error);
    if (bytesPerLine)
        *bytesPerLine = int(rowPitch);
    if (bytesPerSlice)
        *bytesPerSlice = int(slicePitch);
    if (error == CL_SUCCESS)
        return QCLEvent(event);
    else
        return QCLEvent();
}

QT_END_NAMESPACE
