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

#include "qclcommandqueue.h"
#include "qclcontext.h"
#include <QtCore/qvarlengtharray.h>

QT_BEGIN_NAMESPACE

/*!
    \class QCLCommandQueue
    \brief The QCLCommandQueue class represents an OpenCL command queue on a QCLContext.
    \since 4.7
    \ingroup opencl

    QCLCommandQueue is just a handle to a command queue.  Commands
    are added to the queue by calling methods on QCLContext,
    QCLBuffer, QCLImage2D, QCLKernel, etc.  These methods use
    QCLContext::commandQueue() as the command destination.
    QCLContext::setCommandQueue() can be used to alter the
    destination queue.
*/

/*!
    \fn QCLCommandQueue::QCLCommandQueue()

    Constructs a null OpenCL command queue object.
*/

/*!
    \fn QCLCommandQueue::QCLCommandQueue(QCLContext *context, cl_command_queue id)

    Constructs an OpenCL command queue object based on the supplied
    native OpenCL \a id, and associates it with \a context.  This class
    will take over ownership of \a id and release it in the destructor.
*/

/*!
    Constructs a copy of \a other.
*/
QCLCommandQueue::QCLCommandQueue(const QCLCommandQueue &other)
    : m_context(other.m_context), m_id(other.m_id)
{
    if (m_id)
        clRetainCommandQueue(m_id);
}

/*!
    Releases this OpenCL command queue.  If this object is the
    last reference, the queue will be destroyed.
*/
QCLCommandQueue::~QCLCommandQueue()
{
    if (m_id)
        clReleaseCommandQueue(m_id);
}

/*!
    Assigns \a other to this object.
*/
QCLCommandQueue &QCLCommandQueue::operator=(const QCLCommandQueue &other)
{
    m_context = other.m_context;
    if (m_id == other.m_id)
        return *this;
    if (m_id)
        clReleaseCommandQueue(m_id);
    m_id = other.m_id;
    if (m_id)
        clRetainCommandQueue(m_id);
    return *this;
}

/*!
    \fn bool QCLCommandQueue::isNull() const

    Returns true if this OpenCL command queue is null.
*/

/*!
    Returns true if this command queue executes commands out of order;
    otherwise false if commands are executed in order.

    \sa setOutOfOrder()
*/
bool QCLCommandQueue::isOutOfOrder() const
{
    if (!m_id)
        return false;
    cl_command_queue_properties props = 0;
    if (clGetCommandQueueInfo(m_id, CL_QUEUE_PROPERTIES,
                              sizeof(props), &props, 0) != CL_SUCCESS)
        return false;
    return (props & CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE) != 0;
}

/*!
    Enables or disables out of order execution of commands according
    to \a enable.

    \sa isOutOfOrder()
*/
void QCLCommandQueue::setOutOfOrder(bool enable)
{
    if (!m_id)
        return;
    clSetCommandQueueProperty(m_id, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE,
                              (enable ? CL_TRUE : CL_FALSE), 0);
}

/*!
    \fn cl_command_queue QCLCommandQueue::id() const

    Returns the native OpenCL command queue identifier for this object.
*/

/*!
    \fn QCLContext *QCLCommandQueue::context() const

    Returns the OpenCL context that created this queue object.
*/

/*!
    \fn bool QCLCommandQueue::operator==(const QCLCommandQueue &other) const

    Returns true if this OpenCL command queue is the same as \a other;
    false otherwise.

    \sa operator!=()
*/

/*!
    \fn bool QCLCommandQueue::operator!=(const QCLCommandQueue &other) const

    Returns true if this OpenCL command queue is not the same as \a other;
    false otherwise.

    \sa operator==()
*/

QT_END_NAMESPACE
