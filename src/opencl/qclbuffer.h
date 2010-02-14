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

#ifndef QCLBUFFER_H
#define QCLBUFFER_H

#include "qclmemoryobject.h"
#include "qclevent.h"
#include <QtCore/qrect.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(CL)

class QCLImage2D;
class QCLImage3D;

class Q_CL_EXPORT QCLBuffer : public QCLMemoryObject
{
public:
    QCLBuffer() {}
    QCLBuffer(QCLContext *context, cl_mem id)
        : QCLMemoryObject(context, id) {}
    QCLBuffer(const QCLBuffer& other)
        : QCLMemoryObject(other.context(), other.id()) {}

    QCLBuffer& operator=(const QCLBuffer& other)
    {
        setId(other.context(), other.id());
        return *this;
    }

    bool read(size_t offset, void *data, size_t size);
    QCLEvent readAsync(size_t offset, void *data, size_t size,
                       const QVector<QCLEvent>& after = QVector<QCLEvent>());

    bool write(size_t offset, const void *data, size_t size);
    QCLEvent writeAsync(size_t offset, const void *data, size_t size,
                        const QVector<QCLEvent>& after = QVector<QCLEvent>());

    bool copyTo(size_t offset, size_t size,
                const QCLBuffer& dest, size_t destOffset);
    bool copyTo(size_t offset, const QCLImage2D& dest, const QRect& rect);
    bool copyTo(size_t offset, const QCLImage3D& dest,
                const size_t origin[3], const size_t size[3]);
    QCLEvent copyToAsync
        (size_t offset, size_t size,
         const QCLBuffer& dest, size_t destOffset,
         const QVector<QCLEvent>& after = QVector<QCLEvent>());
    QCLEvent copyToAsync
        (size_t offset, const QCLImage2D& dest, const QRect& rect,
         const QVector<QCLEvent>& after = QVector<QCLEvent>());
    QCLEvent copyToAsync
        (size_t offset, const QCLImage3D& dest,
         const size_t origin[3], const size_t size[3],
         const QVector<QCLEvent>& after = QVector<QCLEvent>());

    void *map(size_t offset, size_t size, QCLMemoryObject::MapAccess access);
    QCLEvent mapAsync(void **ptr, size_t offset, size_t size,
                      QCLMemoryObject::MapAccess access,
                      const QVector<QCLEvent>& after = QVector<QCLEvent>());
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
