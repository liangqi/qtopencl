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

#ifndef QCLCONTEXT_H
#define QCLCONTEXT_H

#include "qcldevice.h"
#include "qclcommandqueue.h"
#include "qclbuffer.h"
#include "qclimage.h"
#include "qclsampler.h"
#include "qclprogram.h"
#include <QtCore/qscopedpointer.h>
#include <QtCore/qsize.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qstring.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(CL)

class QCLContextPrivate;
class QCLKernel;

class Q_CL_EXPORT QCLContext
{
public:
    QCLContext();
    virtual ~QCLContext();

    bool isCreated() const;

    QCLPlatform platform() const;
    void setPlatform(const QCLPlatform& platform);

    bool create(QCLDevice::DeviceTypes type = QCLDevice::GPU);
    bool create(const QList<QCLDevice>& devices);
    void release();

    cl_context id() const;
    void setId(cl_context id);

    QList<QCLDevice> devices() const;
    QCLDevice defaultDevice() const;

    cl_int lastError() const;
    void setLastError(cl_int error);

    static QString errorName(cl_int code);

    QCLCommandQueue commandQueue();
    void setCommandQueue(const QCLCommandQueue& queue);

    QCLCommandQueue defaultCommandQueue();
    QCLCommandQueue createCommandQueue
        (const QCLDevice& device, cl_command_queue_properties properties);

    QCLBuffer createBufferDevice
        (size_t size, QCLMemoryObject::Access access);
    QCLBuffer createBufferHost
        (void *data, size_t size, QCLMemoryObject::Access access);
    QCLBuffer createBufferCopy
        (const void *data, size_t size, QCLMemoryObject::Access access);

    QCLImage2D createImage2DDevice
        (const QCLImageFormat& format, const QSize& size,
         QCLMemoryObject::Access access);
    QCLImage2D createImage2DHost
        (const QCLImageFormat& format, void *data, const QSize& size,
         QCLMemoryObject::Access access, int bytesPerLine = 0);
    QCLImage2D createImage2DHost
        (QImage *image, QCLMemoryObject::Access access);
    QCLImage2D createImage2DCopy
        (const QCLImageFormat& format, const void *data, const QSize& size,
         QCLMemoryObject::Access access, int bytesPerLine = 0);
    QCLImage2D createImage2DCopy
        (const QImage& image, QCLMemoryObject::Access access);

    QCLImage3D createImage3DDevice
        (const QCLImageFormat& format, int width, int height, int depth,
         QCLMemoryObject::Access access);
    QCLImage3D createImage3DHost
        (const QCLImageFormat& format, void *data,
         int width, int height, int depth,
         QCLMemoryObject::Access access,
         int bytesPerLine = 0, int bytesPerSlice = 0);
    QCLImage3D createImage3DCopy
        (const QCLImageFormat& format, const void *data,
         int width, int height, int depth,
         QCLMemoryObject::Access access,
         int bytesPerLine = 0, int bytesPerSlice = 0);

    QCLSampler createSampler
        (bool normalizedCoords, cl_addressing_mode addressingMode,
         cl_filter_mode filterMode);

    QCLProgram createProgramFromSourceCode(const char *sourceCode);
    QCLProgram createProgramFromSourceCode(const QByteArray& sourceCode);
    QCLProgram createProgramFromSourceFile(const QString& fileName);

    QCLProgram buildProgramFromSourceCode(const char *sourceCode);
    QCLProgram buildProgramFromSourceCode(const QByteArray& sourceCode);
    QCLProgram buildProgramFromSourceFile(const QString& fileName);

    QList<QCLImageFormat> supportedImage2DFormats(cl_mem_flags flags) const;
    QList<QCLImageFormat> supportedImage3DFormats(cl_mem_flags flags) const;

    void flush();
    void finish();

    QCLEvent marker();

    void sync();

    void barrier();
    void barrier(const QVector<QCLEvent>& events);

private:
    QScopedPointer<QCLContextPrivate> d_ptr;

    Q_DISABLE_COPY(QCLContext)
    Q_DECLARE_PRIVATE(QCLContext)

    cl_command_queue activeQueue(); // For quicker access from friends.

    friend class QCLMemoryObject;
    friend class QCLBuffer;
    friend class QCLImage2D;
    friend class QCLImage3D;
    friend class QCLKernel;
    friend class QCLCommandQueue;
    friend class QCLProgram;

    void reportError(const char *name, cl_int error);
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
