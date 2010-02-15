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
QCLCommandQueue::QCLCommandQueue(const QCLCommandQueue& other)
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
QCLCommandQueue& QCLCommandQueue::operator=(const QCLCommandQueue& other)
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
    Flushes all previously queued commands to the device associated
    with this command queue.  The commands are delivered to the device,
    but no guarantees are given that they will be executed.

    \sa finish()
*/
void QCLCommandQueue::flush()
{
    if (m_id)
        clFlush(m_id);
}

/*!
    Blocks until all previously queued commands have finished execution.

    \sa flush()
*/
void QCLCommandQueue::finish()
{
    if (m_id)
        clFinish(m_id);
}

/*!
    Returns a marker event for this command queue.  The event will
    be signalled when all commands that were queued before this
    point have been completed.

    \sa barrier()
*/
QCLEvent QCLCommandQueue::marker()
{
    cl_event evid;
    if (!m_id)
        return QCLEvent();
    cl_int error = clEnqueueMarker(m_id, &evid);
    context()->reportError("QCLCommandQueue::marker:", error);
    if (error != CL_SUCCESS)
        return QCLEvent();
    else
        return QCLEvent(evid);
}

/*!
    Adds a barrier to this command queue.  All commands that were
    queued before this point must complete before any further
    commands added after this point are executed.

    \sa marker()
*/
void QCLCommandQueue::barrier()
{
    if (!m_id)
        return;
    cl_int error = clEnqueueBarrier(m_id);
    context()->reportError("QCLCommandQueue::barrier:", error);
}

/*!
    Adds a barrier to this command queue that will prevent future
    commands from being executed until after \a event is signalled.

    \sa marker()
*/
void QCLCommandQueue::barrier(const QCLEvent& event)
{
    cl_event evtid = event.id();
    if (!m_id || !evtid)
        return;
    cl_int error = clEnqueueWaitForEvents(m_id, 1, &evtid);
    context()->reportError("QCLCommandQueue::barrier(QCLEvent):", error);
}

/*!
    Adds a barrier to this command queue that will prevent future
    commands from being executed until after all members of \a events
    have been signalled.

    \sa marker()
*/
void QCLCommandQueue::barrier(const QVector<QCLEvent>& events)
{
    if (!m_id || events.isEmpty())
        return;
    cl_int error = clEnqueueWaitForEvents
        (m_id, events.size(),
         reinterpret_cast<const cl_event *>(events.constData()));
    context()->reportError("QCLCommandQueue::barrier(QVector<QCLEvent>):", error);
}

QT_END_NAMESPACE
